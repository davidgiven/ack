/* $Id$ */

#define DEBUGGEE
#include "message.h"

#define MAXBP	128
#define MAXTRACE 16

static int	nbp;			/* # of breakpoints */
static char	*bp[MAXBP];
static int	ntrace;
static struct trace {
  char	*begin_trace, *end_trace;
} trace_buf[MAXTRACE];

static struct message_hdr ok_message;

#define OFF	0
#define SS	1			/* single stepping */
#define SSF	2			/* single stepping, counting functions
					   as single statements */

static char *
BUFTOA(p)
  register char	*p;
{
  register long	l = 0;
  register int i;

  for (i = PS; i>0; i--) {
	l = (l << 8) | (*p++ & 0377);
  }
  return (char *) l;
}

static long
BUFTOL(p)
  register char	*p;
{
  register long	l = 0;
  register int i;

  for (i = LS; i>0; i--) {
	l = (l << 8) | (*p++ & 0377);
  }
  return l;
}

static
ATOBUF(p, cp)
  register char	*p;
  char *cp;
{
  register int i;
  register long l = (long) cp;

  p += PS;
  for (i = PS; i > 0; i--) {
	*--p = l;
	l >>= 8;
  }
}

static
LTOBUF(p, l)
  register char	*p;
  register long	l;
{
  register int i;

  p += LS;
  for (i = LS; i > 0; i--) {
	*--p = l;
	l >>= 8;
  }
}

static int single_stepping = OFF;
static int db_ss = 0;
static int step_count = 0;
static char	*savedlb;
static int	break_flag = 0;
static int	reading = 0;
static char	*retaddr;
static char	*LB;
static char	*currbrk;

extern char *__Get_LB();
extern char *__Get_Hol0();
extern char *__Get_PC();
extern char *__Cvt_LB_AB();
extern char *__Get_HP();
extern char *_sbrk();
extern int  etext;
extern __Set_PC();
static send_ok();

#define	check_ptr(c)	(((c) >= (char *)&etext && (c) <= currbrk) || (c) >= LB)

#define IN_FD	3
#define OUT_FD	6

static int
ureceive(p, c)
  char	*p;
  long	c;
{
  int	i;
  int	retval = 1;

  reading = 1;
  while (c >= 0x1000) {
	i = _read(IN_FD, p, 0x1000);
	if (i == 0) return 0;
	if (i < 0) {
		retval = 0;
		c -= 0x1000;
	}
	else {
		p += i;
		c -= i;
	}
  }
  while (c > 0) {
	i = _read(IN_FD, p, (int)c);
	if (i == 0) return 0;
	if (i < 0) {
		retval = 0;
		break;
	}
	else {
		p += i;
		c -= i;
	}
  }
  reading = 0;
  return retval;
}

static int
usend(p, c)
  char	*p;
  long	c;
{
  int	i;

  while (c >= 0x1000) {
	i = _write(OUT_FD, p, 0x1000);
	if (i < 0) {
		return 0;
	}
	p += i;
	c -= i;
  }
  while (c > 0) {
	i = _write(OUT_FD, p, (int)c);
	if (i < 0) {
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
  return 1;
}

static int
uputm(message)
  struct message_hdr *message;
{
  if (! usend((char *) message, (long) sizeof(struct message_hdr))) {
  	return 0;
  }
  return 1;
}

static int
sendreply(m, p, sz)
  struct message_hdr *m;
  char *p;
  long sz;
{
  if (! uputm(m) || ! (sz && p ? usend(p, sz) : 1)) {
	return 0;
  }
  return 1;
}

static int
do_request()
{
  /* obtain a request from the debugger and perform it */
  int fail = 0;
  register int i;
  register char *c;
  char *c1;
  long sz;
  struct message_hdr message;

start:
  if (! ugetm(&message)) {
	/* failed to get a message. Something is wrong. Let process continue */
	return 1;
  }

  if (message.m_type & M_DB_SS) db_ss = 1;
  message.m_type &= 0177;
  switch(message.m_type) {
  case M_OK:
	/* sometimes sent to child to see if it lives */
	goto start;
  case M_SETBP:
	/* set a breakpoint */
	if (nbp == MAXBP) {
		fail = 1;
		break;
	}
	bp[nbp++] = BUFTOA(message.m_buf+1);
	break;
  case M_SETTRACE:
	if (ntrace == MAXTRACE) {
		fail = 1;
		break;
	}
	trace_buf[ntrace].begin_trace = BUFTOA(message.m_buf+1);
	trace_buf[ntrace++].end_trace = BUFTOA(message.m_buf+(PS+1));
	break;
  case M_CLRBP:
	i = 0;
	c = BUFTOA(message.m_buf+1);
	while (i < nbp && bp[i] != c) {
		  i++;
	}
	if (i < nbp) {
		while (i < nbp) {
			bp[i] = bp[i+1];
			i++;
		}
		nbp--;
	}
	else {
		fail = 1;
	}
	break;
  case M_CLRTRACE:
	i = 0;
	c = BUFTOA(message.m_buf+1);
	c1 = BUFTOA(message.m_buf+(PS+1));
	while (i < ntrace &&
	       trace_buf[i].begin_trace != c &&
	       trace_buf[i].end_trace != c1){
		  i++;
	}
	if (i < ntrace) {
		while (i < ntrace) {
			trace_buf[i] = trace_buf[i+1];
			i++;
		}
		ntrace--;
	}
	else {
		fail = 1;
	}
	break;
  case M_SETSS:
	single_stepping = SS;
	step_count = BUFTOL(message.m_buf+1);
	return 1;
  case M_CLRSS:
	single_stepping = OFF;
	break;
  case M_SETSSF:
	savedlb = __Get_LB(2);
	step_count = BUFTOL(message.m_buf+1);
	single_stepping = SSF;
	return 1;
  case M_GETBYTES:
	message.m_type = M_DATA;
	sz = BUFTOL(message.m_buf+1);
	c = BUFTOA(message.m_buf+(LS+1));
        currbrk = _sbrk(0);
	if (! check_ptr(c) || ! check_ptr(c+sz-1)) {
		fail = 1;
		break;
	}
	return sendreply(&message, c, sz)
		? 0
		: -1;

  case M_GETSTR:
	sz = BUFTOL(message.m_buf+1);
	c1 = c = BUFTOA(message.m_buf+(LS+1));
        currbrk = _sbrk(0);
	if (check_ptr(c)) {
		while (*c) {
			if (c - c1 + 1 >= sz) break;
			c++;
			if (! check_ptr(c)) {
				fail = 1;
				break;
			}
		}
	}
	else	fail = 1;
	if (fail) break;
	message.m_type = M_DATA;
	sz = c - c1 + 1;
	LTOBUF(message.m_buf+1, sz);
	return sendreply(&message, c1, sz)
		? 0
		: -1;

  case M_SETBYTES:
	if (! ureceive(BUFTOA(message.m_buf+(LS+1)), BUFTOL(message.m_buf+1))) {
		fail = 1;
	}
	break;

  case M_GETEMREGS:
	i = BUFTOL(message.m_buf+1);
	c = __Get_LB(i+2);		/* local base */
	c1 = __Get_LB(i+1);		/* needed to find PC and SP */
	ATOBUF(message.m_buf+LB_OFF, c);
	ATOBUF(message.m_buf+AB_OFF, __Cvt_LB_AB(c));
	ATOBUF(message.m_buf+PC_OFF, __Get_PC(c1));
	ATOBUF(message.m_buf+HP_OFF, __Get_HP());
	ATOBUF(message.m_buf+SP_OFF, __Cvt_LB_AB(c1));
	return sendreply(&message, (char *) 0, 0L) ? 0 : -1;

  case M_SETEMREGS:
	/* actually, only the program counter is settable */
	i = BUFTOL(message.m_buf+1);
	c = __Get_LB(i+1);
	__Set_PC(c, BUFTOA(message.m_buf+PC_OFF));
	break;

  case M_CONT:
	return 1;

  case M_DUMP:
	currbrk = _sbrk(0);
	c = __Get_Hol0();
	c1 = currbrk;
	message.m_type = M_DGLOB;
	sz = c1 - c;
	LTOBUF(message.m_buf+1, sz);
	ATOBUF(message.m_buf + HP_OFF, __Get_HP());
	ATOBUF(message.m_buf + PC_OFF, __Get_PC(__Get_LB(1)));
	ATOBUF(message.m_buf + LB_OFF, c1);

	sendreply(&message, c, sz);
	if (! ___topsave()) {
		/* we get here after a ___topload() ! */
		step_count = 0;
		nbp = 0;
		single_stepping = 0;
		savedlb = 0;
		break_flag = 0;
		db_ss = 0;
		break;
	}
	return 0;
  case M_DGLOB:
	_brk(BUFTOA(message.m_buf + LB_OFF));
	__Set_HP(BUFTOA(message.m_buf+HP_OFF));
	if (! ureceive(__Get_Hol0(), BUFTOL(message.m_buf+1))) {}
	___topload(BUFTOA(message.m_buf + SP_OFF));
	/*NOTREACHED*/
  }
  send_ok(fail ? M_FAIL : M_OK);
  return fail ? -1 : 0;
}

___sendtop(addr, SP, sz)
  char *addr, *SP;
  unsigned sz;
{
  struct message_hdr m;

  m.m_type = M_DSTACK;
  LTOBUF(m.m_buf+1, (long) sz);
  ATOBUF(m.m_buf+SP_OFF, SP);
  sendreply(&m, addr, (long)sz);
}

___restoretop(SP)
  char *SP;
{
  struct message_hdr m;

  if (! ugetm(&m)) {}
  if (! ureceive(SP, BUFTOL(m.m_buf+1))) {}
}

static
sigcatch()
{
  signal(7, sigcatch);
  if (reading) send_ok(M_INTR);
  else break_flag = 1;
  db_ss = 0;
}

static int
check_bp()
{
  register int i;

  for (i = 0; i < nbp; i++) {
    	if (bp[i] == retaddr) {
		return 1;
	}
  }
  return 0;
}

static int
check_trace()
{
  register int i;

  for (i = 0; i < ntrace; i++) {
	if (trace_buf[i].begin_trace <= retaddr &&
	    trace_buf[i].end_trace >= retaddr) {
		return 1;
	}
  }
  return 0;
}

static
send_ok(type)
{
  register int i;

  ok_message.m_type = type;
  LTOBUF(ok_message.m_buf+1, (long) retaddr);
  uputm(&ok_message);
}

void
__uX_LiB()
/* This function must be called after every LIN and LNI */
{
  int status = M_CONT;

  LB = __Get_LB(0);
  if ( retaddr) {
	/* not the first time ... */
  	retaddr = __Get_PC(LB);

	/* first, adjust step_count when single stepping
	*/
	if (single_stepping == SS) step_count--;
	else if (single_stepping == SSF) {
		char *lb = __Get_LB(1);
		if (!((char *) &lb < lb && lb >= savedlb ||
	      	    (char *) &lb > lb && lb <= savedlb)) {
		}
		else {
			savedlb = lb;
			step_count--;
		}
	}

	if (single_stepping != OFF && step_count <= 0) {
		status = M_END_SS;
		single_stepping = OFF;
	}
	else if (break_flag) status = M_INTR;
  }
  else {
	signal(7, sigcatch);
  	retaddr = __Get_PC(LB);
	status = M_OK;
  }

  if (status == M_CONT) {
	if ((nbp != 0 && check_bp() != 0) ||
  	    (ntrace != 0 && check_trace() != 0)) {
		status = M_OK;
	}
	else if (break_flag) status = M_INTR;
	else {
		if (db_ss) {
			db_ss = 0;
			send_ok(M_DB_SS);
  			while (do_request() <= 0) /* nothing */;
		}
  		if (break_flag) status = M_INTR;
		else return;
	}
  }

  break_flag = 0;
  send_ok(status);
  db_ss = 0;
  while (do_request() <= 0) /* nothing */;
}
