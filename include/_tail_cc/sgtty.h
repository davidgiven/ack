/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* Data structures for ioctl/stty/gtty, sufficient for ACK libraries */

struct sgttyb {
  char sg_ispeed;		/* input speed (not used) */
  char sg_ospeed;		/* output speed (not used) */
  char sg_erase;		/* erase character */
  char sg_kill;			/* kill character */
#ifdef __USG
  int sg_flags;			/* mode flags */
#else
  short sg_flags;		/* mode flags */
#endif
};

struct tchars {
  char t_intrc;			/* SIGINT char */
  char t_quitc;			/* SIGQUIT char */
  char t_startc;		/* start output (initially CTRL-Q) */
  char t_stopc;			/* stop output	(initially CTRL-S) */
  char t_eofc;			/* EOF (initially CTRL-D) */
  char t_brkc;			/* input delimiter (like nl) */
};

/* Fields in t_flags. */
#define ALLDELAY     0177400

#define BSDELAY      0100000
#  define BS0        0000000
#  define BS1        0100000

#define VTDELAY	     0040000
#  define FF0        0000000
#  define FF1        0040000

#define CRDELAY      0030000
#  define CR0        0000000
#  define CR1        0010000
#  define CR2        0020000
#  define CR3        0030000

#define XTABS	     0006000	/* do tab expansion */

#define TBDELAY	     0006000
#  define TAB0       0000000
#  define TAB1       0002000
#  define TAB2       0004000

#define NLDELAY	     0001400
#  define NL0	     0000000
#  define NL1        0000400
#  define NL2        0001000
#  define NL3        0001400

#define ANYP         0000300
#define EVENP        0000200
#define ODDP         0000100

#define RAW	     0000040	/* enable raw mode */
#define CRMOD	     0000020	/* map lf to cr + lf */
#define ECHO	     0000010	/* echo input */
#define LCASE	     0000004
#define CBREAK	     0000002	/* enable cbreak mode */
#define TANDEM	     0000001
/*#define COOKED       0000000	/* neither CBREAK nor RAW */

#ifdef __BDS4_2
#define TIOCGETP (('t'<<8) | 8 | (6 << 16) | 0x40000000)
#define TIOCSETP (('t'<<8) | 9 | (6 << 16) | 0x80000000)
#define TIOCSETN (('t'<<8) | 10 | (6 << 16) | 0x80000000)
#define TIOCEXCL (('t'<<8) | 13 | 0x20000000)
#define TIOCNXCL (('t'<<8) | 14 | 0x20000000)
#define TIOCHPCL (('t'<<8) | 2 | 0x20000000)
#define TIOCGETC (('t'<<8) | 18 | (6 << 16) | 0x40000000)
#define TIOCSETC (('t'<<8) | 17 | (6 << 16) | 0x80000000)
#else
#define TIOCGETP (('t'<<8) | 8)
#define TIOCSETP (('t'<<8) | 9)
#define TIOCSETN (('t'<<8) | 10)
#define TIOCEXCL (('t'<<8) | 13)
#define TIOCNXCL (('t'<<8) | 14)
#define TIOCHPCL (('t'<<8) | 2)
#define TIOCGETC (('t'<<8) | 18)
#define TIOCSETC (('t'<<8) | 17)
#endif

#define B0      0
#define B50     1
#define B75     2
#define B110    3
#define B134    4
#define B150    5
#define B200    6
#define B300    7
#define B600    8
#define B1200   9
#define B1800   10
#define B2400   11
#define B4800   12
#define B9600   13
#define EXTA    14
#define EXTB    15
