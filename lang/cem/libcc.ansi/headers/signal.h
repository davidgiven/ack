/*
 * signal.h - signal handling
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
#ifndef	_SIGNAL_HEADER_
#define	_SIGNAL_HEADER_

typedef	int		sig_atomic_t;

#define	SIG_ERR		(void (*)())-1
#if	defined(em22) || defined(em24) || defined(em44)
#define	SIG_DFL		((void (*)())-2)
#define	SIG_IGN		((void (*)())-3)
#else
#define	SIG_DFL		((void (*)())0)
#define	SIG_IGN		((void (*)())1)
#endif	/* SIG_ERR */

#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt */
#define	SIGQUIT	3	/* quit */
#define	SIGILL	4	/* illegal instruction (not reset when caught) */
#define	SIGTRAP	5	/* trace trap (not reset when caught) */
#define	SIGIOT	6	/* IOT instruction */
#define	SIGABRT	6	/* ANSI abort trap */
#define	SIGEMT	7	/* EMT instruction */
#define	SIGFPE	8	/* floating point exception */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad argument to system call */
#define	SIGPIPE	13	/* write on a pipe with no one to read it */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* software termination signal from kill */
#ifdef	__USG
#define	SIGUSR1	16	/* user defined signal 1 */
#define	SIGUSR2	17	/* user defined signal 2 */
#define	SIGCLD	18	/* death of a child */
#define	SIGPWR	19	/* power-fail signal */
#define	_NSIG	20
#elif	defined(__BSD4_2)
#define	SIGURG	16	/* urgent condition */
#define	SIGSTOP	17	/* stop signal not from tty */
#define	SIGTSTP	18	/* stop signal from tty */
#define	SIGCONT	19	/* continue a stopped process */
#define	SIGCHLD	20	/* death of a child */
#define	SIGCLD	20	/* System V compat. */
#define	SIGTTIN	21	/* background tty read */
#define	SIGTTOU	22	/* background tty write */
#define	SIGIO	23	/* I/O possible signal */
#define	SIGPOLL	SIGIO	/* System V compat. */
#define	SIGXCPU	24	/* exceeded CPU time limit */
#define	SIGXFSZ	25	/* exceeded file size limit */
#define	SIGVTALRM 26	/* virtual time alarm */
#define	SIGPROF	27	/* profiling time alarm */
#define	SIGWINCH 28	/* window has changed */
#define	SIGLOST 29	/* resource lost */
#define SIGUSR1 30	/* user defined signal 1 */
#define SIGUSR2 31	/* user defined signal 2 */
#define	_NSIG	32
#else
#define	_NSIG	16
#endif	/* __USG or __BSD4_2 */

#ifdef	__BSD4_2
struct	sigvec {
	int	(*sv_handler)();	/* handler */
	int	sv_mask;		/* mask to apply */
	int	sv_flags;
};
#define	SV_ONSTACK	0x0001		/* take signal on signal stack */
#define	SV_INTERRUPT	0x0002		/* do not restart system on signal return */
#define	SV_RESETHAND	0x0004		/* reset signal handler to SIG_DFL when signal taken */
#define	sv_onstack	sv_flags	/* compat. */

struct	sigstack {
	char	*ss_sp;			/* signal stack pointer */
	int	ss_onstack;		/* current status */
};

struct	sigcontext {
	int	sc_onstack;		/* sigstack state to restore */
	int	sc_mask;		/* signal mask to restore */
	int	sc_sp;			/* sp to restore */
#ifdef	vax
	int	sc_fp;			/* fp to restore */
	int	sc_ap;			/* ap to restore */
#endif	/* vax */
	int	sc_pc;			/* pc to retore */
	int	sc_ps;			/* psl to restore */
};

#define	BADSIG	SIG_ERR			/* compat. */
#define	sigmask(m)	(1 << ((m)-1))

/*
 * These are only defined for shutting up the compiler.
 */
#ifdef	__STDC__
int	sigvec(int sig, struct sigvec *vec, struct sigvec *ovec);
int	sigstack(struct sigstack *ss, struct sigstack *oss);
int	sigblock(int mask);
int	sigsetmask(int mask);
int	sigpause(int sigmask);
#endif	/* __STDC__ */
#endif	/* __BSD4_2 */

#ifdef	__STDC__
void	(*signal(int sig, void (*func)(int)))(int);
int	raise(int sig);
#endif	/* __STDC__ */

#endif	/* _SIGNAL_HEADER_ */
