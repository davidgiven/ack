/* $Header$ */

#if	!defined(_SGTTY_H)
#define	_SGTTY_H
/* Data structures for IOCTL. */

struct sgttyb {
	char	sg_ispeed;	/* input speed */
	char	sg_ospeed;	/* output speed */
	char	sg_erase;	/* erase character */
	char	sg_kill;	/* kill character */
#if	defined(__USG) && !defined(_XENIX)
	int	sg_flags;	/* mode flags */
#else
	short	sg_flags;	/* mode flags */
#endif
};

struct tchars {
	char	t_intrc;	/* SIGINT char */
	char	t_quitc;	/* SIGQUIT char */
	char	t_startc;	/* start output (initially CTRL-Q) */
	char	t_stopc;	/* stop output	(initially CTRL-S) */
	char	t_eofc;		/* EOF (initially CTRL-D) */
	char	t_brkc;		/* input delimiter (like nl) */
};

/* Field names */
#if	defined(__USG) && !defined(_XENIX)
#define	XTABS		0000002		/* do tab expansion */
#else
#define	XTABS		0006000		/* do tab expansion */
#endif
#define	EVENP		0000200		/* even parity */
#define	ODDP		0000100		/* odd parity */
#define	RAW		0000040		/* enable raw mode */
#define	CRMOD		0000020		/* map lf to cr + lf */
#define	ECHO		0000010		/* echo input */
#define	LCASE		0000004		/* map upper case to lower on input */
#define	CBREAK		0000002		/* enable cbreak mode */
#if	defined(__BSD4_2) || defined(_XENIX)
#define	TANDEM		0000001		/* automatic flow control */
#else
#define	HUPCL		0000001		/* unused ??? */
#endif
#define	COOKED		0000000		/* neither CBREAK nor RAW */

#define	B0		0
#define	B50		1
#define	B75		2
#define	B110		3
#define	B134		4
#define	B150		5
#define	B200		6
#define	B300		7
#define	B600		8
#define	B1200		9
#define	B1800		10
#define	B2400		11
#define	B4800		12
#define	B9600		13
#define	B19200		14
#define	B38400		15
#define	EXTA		14
#define	EXTB		15

#define	TIOCGETP	(('t'<<8) | 8)
#define	TIOCSETP	(('t'<<8) | 9)
#define	TIOCGETC	(('t'<<8) | 18)
#define	TIOCSETC	(('t'<<8) | 17)

#endif	/* _SGTTY_H */
