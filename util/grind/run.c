/* $Id$ */

/* Running a process and communication */

#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <alloc.h>
#include <errno.h>

#include "ops.h"
#include "message.h"
#include "position.h"
#include "tree.h"
#include "file.h"
#include "symbol.h"
#include "idf.h"
#include "scope.h"
#include "type.h"
#include "expr.h"
#include "misc.h"

#define MAXARG	128

extern char	*strncpy();
extern char	*malloc();
extern struct idf *str2idf();

extern char	*AObj;
extern FILE	*db_out;
extern int	debug;
extern char	*progname;
extern int	child_interrupted;
extern int	interrupted;
extern t_lineno	currline;

static int	child_pid;		/* process id of child */
static int	to_child, from_child;	/* file descriptors for communication */
static int	child_status;
static int	restoring;
static int	fild1[2], fild2[2];	/* pipe file descriptors */

int		disable_intr = 1;
int		db_ss;
int		stack_offset;

void		signal_child();

static void	catch_sigpipe();
static int	stopped();
static int	uputm(), ugetm();
static t_addr	curr_stop;
p_tree		run_command;

static void
ITOBUF(p, l, sz)
  register char	*p;
  long	l;
  int sz;
{
  register int i;

  p +=sz;
  for (i = sz; i > 0; i--) {
	*--p = l;
	l >>= 8;
  }
}

static long
BUFTOI(p, sz)
  register char	*p;
  int sz;
{
  register long	l = 0;
  register int i;

  for (i = sz; i>0; i--) {
	l = (l << 8) | (*p++ & 0377);
  }
  return l;
}

void
init_run()
{
  /* take file descriptors so that listing cannot take them */
  int i;

  for (i = 3; i <= 6; i++) close(i);
  if (pipe(fild1) < 0 ||
      pipe(fild2) < 0 ||
      fild1[0] != 3 ||
      fild2[1] != 6) {
	fatal("something wrong with file descriptors");
  }
  to_child = fild1[1];
  from_child = fild2[0];
  child_pid = 0;
  if (currfile) CurrentScope = currfile->sy_file->f_scope;
  currline = 0;
}

extern int errno;

start_child(p)
  p_tree	p;
{
  /* start up the process to be debugged and set up communication */

  char *argp[MAXARG];				/* argument list */
  register p_tree pt = p->t_args[0], pt1 = 0;
  unsigned int	nargs = 1;			/* #args */
  char	*in_redirect = 0;			/* standard input redirected */
  char	*out_redirect = 0;			/* standard output redirected */

  signal_child(SIGKILL);

  if (p != run_command) {
	freenode(run_command);
	run_command = p;
  }
  /* first check arguments and redirections and build argument list */
  while (pt) {
  	switch(pt->t_oper) {
	case OP_LINK:
		pt1 = pt->t_args[1];
		pt = pt->t_args[0];
		continue;
	case OP_NAME:
		if (nargs < (MAXARG-1)) {
			argp[nargs++] = pt->t_str;
		}
		else {
			error("too many arguments");
			return;
		}
		break;
	case OP_INPUT:
		if (in_redirect) {
			error("input redirected twice?");
			return;
		}
		in_redirect = pt->t_str;
		break;
	case OP_OUTPUT:
		if (out_redirect) {
			error("output redirected twice?");
			return;
		}
		out_redirect = pt->t_str;
		break;
  	}
	if (pt != pt1) pt = pt1;
	else break;
  }
  argp[0] = AObj;
  argp[nargs] = 0;

  /* create child process */
  child_pid = fork();
  if (child_pid < 0) {
	error("could not create child");
	return;
  }
  if (child_pid == 0) {
	/* this is the child process */
	close(fild1[1]);
	close(fild2[0]);

	signal(SIGINT, SIG_IGN);

	/* I/O redirection */
	if (in_redirect) {
		int fd;

		close(0);
		if ((fd = open(in_redirect, 0)) < 0 ||
		    (fd != 0 && dup2(fd, 0) < 0)) {
			perror(progname);
			exit(1);
		}
		if (fd != 0) {
			close(fd);
		}
	}
	if (out_redirect) {
		int fd;

		close(1);
		if ((fd = creat(out_redirect, 0666)) < 0 ||
		    (fd != 1 && dup2(fd, 1) < 0)) {
			perror(progname);
			exit(1);
		}
		if (fd != 1) {
			close(fd);
		}
	}

	/* and run process to be debugged */
	execv(AObj, argp);
	error("could not exec %s", AObj);
	exit(1);
  }

  /* close fild1[0] and fild2[1]; but we want those file descriptors occupied,
     so we re-occupy them.
  */

  close(fild1[0]);
  close(fild2[1]);
  pipe(fild1);		/* to occupy file descriptors */

  signal(SIGPIPE, catch_sigpipe);
  {
	struct message_hdr m;

  	if (! ugetm(&m)) {
		error("child not responding");
		init_run();
		return;
	}
	curr_stop = BUFTOI(m.m_buf+1, PS);
	CurrentScope = get_scope_from_addr(curr_stop);
  }
  perform_items();
  if (! restoring) {
	int stop_reason = item_addr_actions(curr_stop, 1, 1);
	if (! stop_reason) (void) send_cont(1);
	else (void) stopped("stopped", curr_stop, stop_reason);
  }
}

void
signal_child(sig)
{
  if (child_pid) {
	kill(child_pid, sig);
	if (sig == SIGKILL) {
		wait(&child_status);
		init_run();
	}
  }
}

static void
catch_sigpipe()
{
  child_pid = 0;
}

extern int errno;
static int
do_read(f, p, c)
  char	*p;
{
  int i = read(f, p, c);

  while (i < 0 && errno == EINTR) i = read(f, p, c);
  return i;
}

static int
ureceive(p, c)
  char	*p;
  long	c;
{
  int	i;
  char buf[0x1000];

  if (! child_pid) {
	error("no process");
  	return 0;
  }

  if (! p) p = buf;
  while (c >= 0x1000) {
	i = do_read(from_child, p, 0x1000);
	if (i <= 0) {
		if (i == 0) {
			child_pid = 0;
		}
		else	error("read failed");
		return 0;
	}
	if (p != buf) p += i;
	c -= i;
  }
  while (c > 0) {
	i = do_read(from_child, p, (int)c);
	if (i <= 0) {
		if (i == 0) {
			child_pid = 0;
		}
		else	error("read failed");
		return 0;
	}
	p += i;
	c -= i;
  }
  return 1;
}

static int
usend(p, c)
  char	*p;
  long	c;
{
  int	i;

  if (! child_pid) {
	error("no process");
	return 0;
  }
  while (c >= 0x1000) {
	i = write(to_child, p, 0x1000);
	if (i < 0) {
		if (child_pid) error("write failed");
		return 0;
	}
	p += i;
	c -= i;
  }
  while (c > 0) {
	i = write(to_child, p, (int)c);
	if (i < 0) {
		if (child_pid) error("write failed");
		return 0;
	}
	p += i;
	c -= i;
  }
  return 1;
}

static int
ugetm(message)
  struct message_hdr *message;
{
  if (! ureceive((char *) message, (long) sizeof(struct message_hdr))) {
  	return 0;
  }
  if (debug) printf("Got %d\n", message->m_type);
  return 1;
}

static int
uputm(message)
  struct message_hdr *message;
{
  if (! usend((char *) message, (long) sizeof(struct message_hdr))) {
  	return 0;
  }
  if (debug) printf("Sent %d\n", message->m_type);
  return 1;
}

static struct message_hdr	answer;
static int	single_stepping;

static int
stopped(s, a, stop_reason)
  char	*s;		/* stop message */
  t_addr a;		/* address where stopped */
  int	stop_reason;	/* status entry causing the stop */
{
  p_position pos;

  if (s && a) {
	fprintf(db_out, "%s ", s);
	pos = print_position(a, 1);
	if (stop_reason) {
		fprintf(db_out, " (status entry %d)", stop_reason);
	}
	fputs("\n", db_out);
	list_position(pos);
	handle_displays();
  }
  curr_stop = a;
  CurrentScope = get_scope_from_addr(a);
  return 1;
}

static int
could_send(m, stop_message)
  struct message_hdr	*m;
{
  int	type;
  t_addr a;
  static int level = 0;
  int child_dead = 0;
  int stop_reason;

  level++;
  for (;;) {
	stop_reason = 0;
  	if (! child_pid) {
		error("no process");
		return 0;
	}
	if (m->m_type & M_DB_RUN) {
		disable_intr = 0;
		stack_offset = 0;
	}
	if (!child_interrupted && (! uputm(m) || ! ugetm(&answer))) {
		child_dead = 1;
	}
	disable_intr = 1;
	if ((interrupted || child_interrupted) && ! child_dead) {
		while (child_interrupted && answer.m_type != M_INTR) {
			if (! ugetm(&answer)) {
				child_dead = 1;
				break;
			}
		}
		if (interrupted && ! child_dead) {
			level--;
			if (! level) {
				child_interrupted = 0;
				interrupted = 0;
				return stopped("interrupted", (t_addr) BUFTOI(answer.m_buf+1, PS), 0);
			}
			return 1;
		}
	}
	if (child_dead) {
		wait(&child_status);
		if (child_status & 0177) {
			fprintf(db_out,
				"child died with signal %d\n",
				child_status & 0177);
		}
		else {
			fprintf(db_out,
				"child terminated, exit status %d\n",
				child_status >> 8);
		}
		init_run();
		level--;
		return 1;
	}
	a = BUFTOI(answer.m_buf+1, PS);
	type = answer.m_type & 0377;
	if (type == M_END_SS) type = 0;
	else if (type == M_OK || type == M_DB_SS) type = 1;
	else type = 2;
	if (m->m_type & M_DB_RUN) {
		/* run command */
		CurrentScope = get_scope_from_addr((t_addr) a);
	    	if (!(stop_reason = item_addr_actions(a, type, stop_message))
		    && (type == 1)) {
			/* no explicit breakpoints at this position.
			   Also, child did not stop because of
			   SETSS or SETSSF, otherwise we would
			   have gotten END_SS.
			   So, continue.
			*/
			if ((m->m_type & 0177) != M_CONT) {
				m->m_type = M_CONT | (m->m_type & M_DB_SS);
			}
			continue;
		}
		if (type != 0 && single_stepping) {
			m->m_type = M_CLRSS;
			if (! uputm(m) || ! ugetm(&answer)) return 0;
		}
		single_stepping = 0;
	}
	level--;
	if (stop_message) {
		return stopped("stopped", a, stop_reason);
	}
	return 1;
  }
  /*NOTREACHED*/
}

static int
getbytes(size, from, to, kind, errmess)
  long	size;
  t_addr from;
  char	*to;
  int kind;
  int errmess;
{
  struct message_hdr	m;

  m.m_type = kind;
  ITOBUF(m.m_buf+1, size, LS);
  ITOBUF(m.m_buf+LS+1, (long)from, PS);

  if (! could_send(&m, 0)) {
	return 0;
  }

  switch(answer.m_type) {
  case M_FAIL:
	if (errmess) error("could not get value");
	return 0;
  case M_INTR:
	if (errmess) error("interrupted");
	return 0;
  case M_DATA:
  	return ureceive(to, BUFTOI(answer.m_buf+1, LS));
  default:
	assert(0);
  }
  /*NOTREACHED*/
}

int
get_bytes(size, from, to)
  long	size;
  t_addr from;
  char	*to;
{
  return getbytes(size, from, to, M_GETBYTES, 1);
}

int
get_string(size, from, to)
  long	size;
  t_addr from;
  char	*to;
{
  int retval = getbytes(size, from, to, M_GETSTR, 0);

  to[(int)BUFTOI(answer.m_buf+1, LS)] = 0;
  return retval;
}

void
set_bytes(size, from, to)
  long	size;
  char	*from;
  t_addr to;
{
  struct message_hdr	m;

  m.m_type = M_SETBYTES;
  ITOBUF(m.m_buf+1, size, LS);
  ITOBUF(m.m_buf+LS+1, (long) to, PS);

  if (! uputm(&m) || ! usend(from, size) || ! ugetm(&m)) {
	return;
  }
  switch(answer.m_type) {
  case M_FAIL:
	error("could not handle this SET request");
	break;
  case M_INTR:
	error("interrupted");
	break;
  case M_OK:
	break;
  default:
	assert(0);
  }
}

t_addr
get_dump(globbuf, stackbuf)
  char **globbuf, **stackbuf;
{
  struct message_hdr	m;
  struct message_hdr *globm, *stackm;
  long sz;

  m.m_type = M_DUMP;
  if (! could_send(&m, 0)) {
	return 0;
  }
  switch(answer.m_type) {
  case M_FAIL:
	error("request for DUMP failed");
	return 0;
  case M_INTR:
	error("interrupted");
	return 0;
  case M_DGLOB:
	break;
  default:
	assert(0);
  }

  sz = BUFTOI(answer.m_buf+1, LS);
  *globbuf = malloc((unsigned) (sz+sizeof(struct message_hdr)));
  if (! *globbuf
      || ! ureceive(*globbuf+sizeof(struct message_hdr), sz)
      || ! ugetm(&m)) {
	if (*globbuf) free(*globbuf);
	else error("could not allocate enougn memory");
	return 0;
  }
  globm = (struct message_hdr *) *globbuf;
  *globm = answer;

  assert(m.m_type == M_DSTACK);
  sz = BUFTOI(m.m_buf+1, LS);
  *stackbuf = malloc((unsigned) sz+sizeof(struct message_hdr));
  if (! *stackbuf || ! ureceive(*stackbuf+sizeof(struct message_hdr), sz)) {
	free(*globbuf);
	if (*stackbuf) free(*stackbuf);
	else error("could not allocate enougn memory");
	return 0;
  }
  stackm = (struct message_hdr *) *stackbuf;
  *stackm = m;
  ITOBUF(globm->m_buf+SP_OFF, BUFTOI(stackm->m_buf+SP_OFF, PS), PS);
  return BUFTOI(globm->m_buf+PC_OFF, PS);
}

int
put_dump(globbuf, stackbuf)
  char *globbuf, *stackbuf;
{
  struct message_hdr m;
  struct message_hdr *globm = (struct message_hdr *) globbuf,
		     *stackm = (struct message_hdr *) stackbuf;

  stackbuf += sizeof(struct message_hdr);
  globbuf += sizeof(struct message_hdr);
  if (! child_pid) {
	restoring = 1;
	start_child(run_command);
	restoring = 0;
  }
  return	uputm(globm)
		&& usend(globbuf, BUFTOI(globm->m_buf+1, LS))
		&& uputm(stackm)
		&& usend(stackbuf, BUFTOI(stackm->m_buf+1, LS))
		&& ugetm(&m)
		&& stopped("restored", BUFTOI(m.m_buf+1, PS), 0);
}

t_addr *
get_EM_regs(level)
  int	level;
{
  struct message_hdr	m;
  static t_addr buf[5];
  register t_addr *to = &buf[0];

  m.m_type = M_GETEMREGS;
  ITOBUF(m.m_buf+1, (long) level, LS);

  if (! could_send(&m, 0)) {
	return 0;
  }
  switch(answer.m_type) {
  case M_FAIL:
	error("request for registers failed");
	return 0;
  case M_INTR:
	error("interrupted");
	return 0;
  case M_GETEMREGS:
	break;
  default:
	assert(0);
  }
  *to++ = (t_addr) BUFTOI(answer.m_buf+LB_OFF, PS);
  *to++ = (t_addr) BUFTOI(answer.m_buf+AB_OFF, PS);
  *to++ = (t_addr) BUFTOI(answer.m_buf+PC_OFF, PS);
  *to++ = (t_addr) BUFTOI(answer.m_buf+HP_OFF, PS);
  *to++ = (t_addr) BUFTOI(answer.m_buf+PC_OFF, PS);
  return buf;
}

int
set_pc(PC)
  t_addr	PC;
{
  struct message_hdr	m;

  m.m_type = M_SETEMREGS;
  ITOBUF(m.m_buf+PC_OFF, (long)PC, PS);
  if (! could_send(&m, 0)) return 0;
  switch(answer.m_type) {
  case M_FAIL:
	error("could not set PC to %lx", (long) PC);
	return 0;
  case M_INTR:
	error("interrupted");
	return 0;
  case M_OK:
	return 1;
  default:
	assert(0);
  }
  /*NOTREACHED*/
}

int
send_cont(stop_message)
  int	stop_message;
{
  struct message_hdr	m;

  m.m_type = (M_CONT | (db_ss ? M_DB_SS : 0));
  return could_send(&m, stop_message) && child_pid;
}

int
singlestep(type, count)
  int	type;
  long	count;
{
  struct message_hdr	m;

  m.m_type = (type ? M_SETSSF : M_SETSS) | (db_ss ? M_DB_SS : 0);
  ITOBUF(m.m_buf+1, count, LS);
  single_stepping = 1;
  if (could_send(&m, 1) && child_pid) return 1;
  single_stepping = 0;
  return 0;
}

int
set_or_clear_breakpoint(a, type)
  t_addr	a;
  int	type;
{
  struct message_hdr m;

  m.m_type = type ? M_SETBP : M_CLRBP;
  ITOBUF(m.m_buf+1, (long) a, PS);
  if (debug) printf("%s breakpoint at 0x%lx\n", type ? "setting" : "clearing", (long) a);
  if (child_pid && ! could_send(&m, 0)) {
  }

  return 1;
}

int
set_or_clear_trace(start, end, type)
  t_addr start, end;
  int	type;
{
  struct message_hdr m;

  m.m_type = type ? M_SETTRACE : M_CLRTRACE;
  ITOBUF(m.m_buf+1, (long)start, PS);
  ITOBUF(m.m_buf+PS+1, (long)end, PS);
  if (debug) printf("%s trace at [0x%lx,0x%lx]\n", type ? "setting" : "clearing", (long) start, (long) end);
  if (child_pid && ! could_send(&m, 0)) {
	return 0;
  }

  return 1;
}
