/* The <sgtty.h> header contains data structures for ioctl(). */

#ifndef _SGTTY_H
#define _SGTTY_H

struct sgttyb {
  char sg_ispeed;		/* input speed */
  char sg_ospeed;		/* output speed */
  char sg_erase;		/* erase character */
  char sg_kill;			/* kill character */
  int  sg_flags;		/* mode flags */
};

struct tchars {
  char t_intrc;			/* SIGINT char */
  char t_quitc;			/* SIGQUIT char */
  char t_startc;		/* start output (initially CTRL-Q) */
  char t_stopc;			/* stop output	(initially CTRL-S) */
  char t_eofc;			/* EOF (initially CTRL-D) */
  char t_brkc;			/* input delimiter (like nl) */
};

/* Field names */
#define XTABS	     0006000	/* do tab expansion */
#define BITS8        0001400	/* 8 bits/char */
#define BITS7        0001000	/* 7 bits/char */
#define BITS6        0000400	/* 6 bits/char */
#define BITS5        0000000	/* 5 bits/char */
#define EVENP        0000200	/* even parity */
#define ODDP         0000100	/* odd parity */
#define RAW	     0000040	/* enable raw mode */
#define CRMOD	     0000020	/* map lf to cr + lf */
#define ECHO	     0000010	/* echo input */
#define CBREAK	     0000002	/* enable cbreak mode */
#define COOKED       0000000	/* neither CBREAK nor RAW */

#define DCD          0100000	/* Data Carrier Detect */

/* Line speeds */
#define B0		   0	/* code for line-hangup */
#define B110		   1
#define B300		   3
#define B1200		  12
#define B2400		  24
#define B4800		  48
#define B9600 		  96

#define TIOCGETP (('t'<<8) | 8)
#define TIOCSETP (('t'<<8) | 9)
#define TIOCGETC (('t'<<8) | 18)
#define TIOCSETC (('t'<<8) | 17)
#define TIOCFLUSH (('t'<<8) | 16)

/* Things Minix supports but not properly */
/* the divide-by-100 encoding ain't too hot */
#define ANYP         0000300
#define B50                0
#define B75                0
#define B134               0
#define B150               0
#define B200               2
#define B600               6
#define B1800             18
#define B3600             36
#define B7200             72
#define EXTA             192
#define EXTB               0

/* Things Minix doesn't support but are fairly harmless if used */
#define NLDELAY      0001400
#define TBDELAY      0006000
#define CRDELAY      0030000
#define VTDELAY      0040000
#define BSDELAY      0100000
#define ALLDELAY     0177400

#if MACHINE == ATARI
/* ST specific clock stuff */

#define	 DCLOCK	('D'<<8)

#define	DC_RBMS100	(DCLOCK|1)
#define	DC_RBMS200	(DCLOCK|2)
#define	DC_RSUPRA	(DCLOCK|3)
#define	DC_RICD  	(DCLOCK|4)
#define	DC_WBMS100	(DCLOCK|8)
#define	DC_WBMS200	(DCLOCK|9)
#endif

#include <ansi.h>

_PROTOTYPE( int gtty, (int _fd, struct sgttyb *_argp)			);
_PROTOTYPE( int ioctl, (int _fd, int _request, struct sgttyb *_argp)	);
_PROTOTYPE( int stty, (int _fd, struct sgttyb *_argp)			);

#endif /* _SGTTY_H */
