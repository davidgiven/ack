/* Data structures for IOCTL. */

struct sgttyb {
  char sg_ispeed;		/* input speed (not used) */
  char sg_ospeed;		/* output speed (not used) */
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

/* Fields in t_flags. */
#define XTABS	     0006000	/* do tab expansion */
#define RAW	     0000040	/* enable raw mode */
#define CRMOD	     0000020	/* map lf to cr + lf */
#define ECHO	     0000010	/* echo input */
#define CBREAK	     0000002	/* enable cbreak mode */
#define COOKED       0000000	/* neither CBREAK nor RAW */

#define TIOCGETP (('t'<<8) | 8)
#define TIOCSETP (('t'<<8) | 9)
#define TIOCGETC (('t'<<8) | 18)
#define TIOCSETC (('t'<<8) | 17)
