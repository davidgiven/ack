/* $Header$ */

/* Running a process and communication */

#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <alloc.h>

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

#define MAXARG	128

extern char	*strncpy();
extern struct idf *str2idf();

extern char	*AObj;
extern FILE	*db_out;
extern int	debug;
extern long	pointer_size;

static int	child_pid;		/* process id of child */
static int	to_child, from_child;	/* file descriptors for communication */
static int	child_status;
static int	restoring;
static int	fild1[2], fild2[2];	/* pipe file descriptors */

int		db_ss;
t_lineno	currline, listline;

static int	catch_sigpipe();
static int	stopped();
static int	uputm(), ugetm();

int
init_run()
{
  /* take file descriptors so that listing cannot take them */
  int i;

  for (i = IN_FD; i <= OUT_FD; i++) close(i);
  if (pipe(fild1) < 0 ||
      pipe(fild2) < 0 ||
      fild1[0] != IN_FD ||
      fild2[1] != OUT_FD) {
	return 0;
  }
  to_child = fild1[1];
  from_child = fild2[0];
  return 1;
}

int
start_child(p)
  p_tree	p;
{
  /* start up the process to be debugged and set up communication */

  char *argp[MAXARG];				/* argument list */
  register p_tree pt = p->t_args[0], pt1;
  unsigned int	nargs = 1;			/* #args */
  char	*in_redirect = 0;			/* standard input redirected */
  char	*out_redirect = 0;			/* standard output redirected */

  signal_child(SIGKILL); /* like families in China, this debugger is only
			    allowed one child
			 */

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
			return 0;
		}
		break;
	case OP_INPUT:
		if (in_redirect) {
			error("input redirected twice?");
			return 0;
		}
		in_redirect = pt->t_str;
		break;
	case OP_OUTPUT:
		if (out_redirect) {
			error("output redirected twice?");
			return 0;
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
	return 0;
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
			error("could not open input file");
			exit(1);
		}
		if (fd != 0) {
			close(fd);
		}
	}
	if (out_redirect) {
		int fd;
		close(1);
		if ((fd = creat(in_redirect, 0666)) < 0 ||
		    (fd != 1 && dup2(fd, 1) < 0)) {
			error("could not open output file");
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

  /* debugger */
  close(fild1[0]);
  close(fild2[1]);

  pipe(fild1);		/* to occupy file descriptors */
  signal(SIGPIPE, catch_sigpipe);
  if (! wait_for_child((char *) 0)) {
	error("child not responding");
	return 0;
  }
  do_items();
  if (! restoring) send_cont(1);
  return 1;
}

int
wait_for_child(s)
  char	*s;		/* to pass on to 'stopped' */
{
  struct message_hdr m;

  if (child_pid) {
  	if (ugetm(&m)) {
		return stopped(s, (t_addr) m.m_size);
  	}
  	return 0;
  }
  return 1;
}

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

static int
catch_sigpipe()
{
  child_pid = 0;
}


static int
ureceive(p, c)
  char	*p;
  long	c;
{
  int	i;
  char buf[0x1000];

  if (! child_pid) return 0;

  if (! p) p = buf;
  while (c >= 0x1000) {
	i = read(from_child, p, 0x1000);
	if (i <= 0) {
		if (i == 0) child_pid = 0;
		return 0;
	}
	if (p != buf) p += i;
	c -= i;
  }
  while (c > 0) {
	i = read(from_child, p, (int)c);
	if (i <= 0) {
		if (i == 0) child_pid = 0;
		return 0;
	}
	p += i;
	c -= i;
  }
  return c == 0;
}

static int
usend(p, c)
  char	*p;
  long	c;
{
  int	i;

  while (c >= 0x1000) {
	i = write(to_child, p, 0x1000);
	if (i < 0) return 0;
	p += i;
	c -= i;
  }
  while (c > 0) {
	i = write(to_child, p, (int)c);
	if (i < 0) return 0;
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
stopped(s, a)
  char	*s;	/* stop message */
  t_addr a;	/* address where stopped */
{
  p_position pos;

  if (s) {
	fprintf(db_out, "%s ", s);
	pos = print_position((t_addr) a, 1);
	fputs("\n", db_out);
	list_position(pos);
  }
  return 1;
}

static int
could_send(m, stop_message)
  struct message_hdr	*m;
{
  int	type;
  t_addr a;
  for (;;) {
  	if (child_pid) {
		if (! uputm(m) ||
		    ! ugetm(&answer)) {
			if (child_pid) {
				error("something wrong!");
				return 1;
			}
			wait(&child_status);
			init_run();
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
			return 1;
		}
		a = answer.m_size;
		type = answer.m_type;
		if (m->m_type & DB_RUN) {
			/* run command */
			CurrentScope = get_scope_from_addr((t_addr) a);
		    	if (! item_addr_actions(a) &&
		            ( type == DB_SS || type == OK)) {
				/* no explicit breakpoints at this position.
				   Also, child did not stop because of
				   SETSS or SETSSF, otherwise we would
				   have gotten END_SS.
				   So, continue.
				*/
				if ((m->m_type & ~ DB_SS) != CONT) {
					m->m_type = CONT | (m->m_type & DB_SS);
				}
				continue;
			}
			if (type != END_SS && single_stepping) {
				m->m_type = CLRSS;
				uputm(m) && ugetm(&answer);
			}
			single_stepping = 0;
		}
		if (stop_message) {
			stopped("stopped", a);
			handle_displays();
		}
		return 1;
	}
	return 0;
  }
  /*NOTREACHED*/
}

int
get_bytes(size, from, to)
  long	size;
  t_addr from;
  char	*to;
{
  struct message_hdr	m;

  m.m_type = GETBYTES;
  m.m_size = size;
  put_int(m.m_buf, pointer_size, (long)from);

  if (! could_send(&m, 0)) {
	return 0;
  }

  if (answer.m_type == FAIL) {
	return 0;
  }

  assert(answer.m_type == DATA && answer.m_size == m.m_size);

  return ureceive(to, answer.m_size);
}

int
set_bytes(size, from, to)
  long	size;
  char	*from;
  t_addr to;
{
  struct message_hdr	m;

  m.m_type = SETBYTES;
  m.m_size = size;
  put_int(m.m_buf, pointer_size, (long) to);

  return uputm(&m)
	 && usend(from, size)
	 && ugetm(&m)
	 && m.m_type != FAIL;
}

int
get_dump(globmessage, globbuf, stackmessage, stackbuf)
  struct message_hdr *globmessage, *stackmessage;
  char **globbuf, **stackbuf;
{
  struct message_hdr	m;

  m.m_type = DUMP;
  if (! could_send(&m, 0)) {
	error("no debuggee");
	return 0;
  }
  if (answer.m_type == FAIL) return 0;
  assert(answer.m_type == DGLOB);
  *globmessage = answer;
  *globbuf = malloc((unsigned) answer.m_size);
  if (! ureceive(*globbuf, answer.m_size) || ! ugetm(stackmessage)) {
	if (*globbuf) free(*globbuf);
	error("no debuggee");
	return 0;
  }
  assert(stackmessage->m_type == DSTACK);
  *stackbuf = malloc((unsigned) stackmessage->m_size);
  if (! ureceive(*stackbuf, stackmessage->m_size)) {
	if (*globbuf) free(*globbuf);
	if (*stackbuf) free(*stackbuf);
	error("no debuggee");
	return 0;
  }
  put_int(globmessage->m_buf+SP_OFF*pointer_size, pointer_size,
	 get_int(stackmessage->m_buf+SP_OFF*pointer_size, pointer_size, T_UNSIGNED));
  if (! *globbuf || ! *stackbuf) {
	error("could not allocate enough memory");
	if (*globbuf) free(*globbuf);
	if (*stackbuf) free(*stackbuf);
	return 0;
  }
  return 1;
}

int
put_dump(globmessage, globbuf, stackmessage, stackbuf)
  struct message_hdr *globmessage, *stackmessage;
  char *globbuf, *stackbuf;
{
  struct message_hdr m;

  if (! child_pid) {
	restoring = 1;
	start_child(run_command);
	restoring = 0;
  }
  return	uputm(globmessage) &&
		usend(globbuf, globmessage->m_size) &&
		uputm(stackmessage) &&
		usend(stackbuf, stackmessage->m_size) &&
		ugetm(&m) && stopped("restored", m.m_size);
}

t_addr *
get_EM_regs(level)
  int	level;
{
  struct message_hdr	m;
  static t_addr buf[5];
  register t_addr *to = &buf[0];

  m.m_type = GETEMREGS;
  m.m_size = level;

  if (! could_send(&m, 0)) {
	return 0;
  }
  if (answer.m_type == FAIL) return 0;
  *to++ = (t_addr) get_int(answer.m_buf, pointer_size, T_UNSIGNED);
  *to++ = (t_addr) get_int(answer.m_buf+pointer_size, pointer_size, T_UNSIGNED);
  *to++ = (t_addr) get_int(answer.m_buf+2*pointer_size, pointer_size, T_UNSIGNED);
  *to++ = (t_addr) get_int(answer.m_buf+3*pointer_size, pointer_size, T_UNSIGNED);
  *to++ = (t_addr) get_int(answer.m_buf+4*pointer_size, pointer_size, T_UNSIGNED);
  return buf;
}

int
set_pc(PC)
  t_addr	PC;
{
  struct message_hdr	m;

  m.m_type = SETEMREGS;
  m.m_size = 0;
  put_int(m.m_buf+PC_OFF*pointer_size, pointer_size, (long)PC);
  return could_send(&m, 0) && answer.m_type != FAIL;
}

int
send_cont(stop_message)
  int	stop_message;
{
  struct message_hdr	m;

  m.m_type = (CONT | (db_ss ? DB_SS : 0));
  m.m_size = 0;
  return could_send(&m, stop_message) && answer.m_type != FAIL;
}

int
do_single_step(type, count)
  int	type;
  long	count;
{
  struct message_hdr	m;

  m.m_type = type | (db_ss ? DB_SS : 0);
  m.m_size = count;
  single_stepping = 1;
  if (could_send(&m, 1) && answer.m_type != FAIL) {
	return 1;
  }
  single_stepping = 0;
  return 0;
}

int
set_or_clear_breakpoint(a, type)
  t_addr	a;
  int	type;
{
  struct message_hdr m;

  if (a == ILL_ADDR || a == NO_ADDR) return 0;

  m.m_type = type;
  m.m_size = a;
  if (debug) printf("%s breakpoint at 0x%lx\n", type == SETBP ? "setting" : "clearing", (long) a);
  if (! could_send(&m, 0)) { }

  return 1;
}

int
set_or_clear_trace(start, end, type)
  t_addr start, end;
  int	type;
{
  struct message_hdr m;

  m.m_type = type;
  put_int(m.m_buf, pointer_size, (long)start);
  put_int(m.m_buf+pointer_size, pointer_size, (long)end);
  if (debug) printf("%s trace at [0x%lx,0x%lx]\n", type == SETTRACE ? "setting" : "clearing", (long) start, (long) end);
  if (! could_send(&m, 0)) { }

  return 1;
}
