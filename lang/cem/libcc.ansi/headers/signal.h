/*
 * signal.h - signal handling
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#if	!defined(_SIGNAL_H)
#define	_SIGNAL_H

typedef	int		sig_atomic_t;

#if	defined(_POSIX_SOURCE)
#if	defined(_MINIX)
typedef	unsigned short sigset_t;

#define	SIG_BLOCK	0	/* for blocking signals */
#define	SIG_UNBLOCK	1	/* for unblocking signals */
#define	SIG_SETMASK	2	/* for setting the signal mask */

struct sigaction {
	void (*sa_handler)(int);/* SIG_DFL, SIG_IGN or pointer to function */
	sigset_t sa_mask;	/* signals blocked during handling */
	int sa_flags;		/* special flags */
};
#endif
#endif

#define	SIG_ERR		((void (*)(int))-1)
#if	defined(__em22) || defined(__em24) || defined(__em44)
#define	SIG_DFL		((void (*)(int))-2)
#define	SIG_IGN		((void (*)(int))-3)
#else
#define	SIG_DFL		((void (*)(int))0)
#define	SIG_IGN		((void (*)(int))1)
#endif	/* no interpretation */

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
#if	defined(__USG)
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
#elif	defined(_MINIX)
/* The following signals are defined but not supported */
#define SIGCHLD		17	/* child process terminated or stopped */
#define SIGCONT		18	/* continue if stopped */
#define SIGSTOP		19	/* stop signal */
#define SIGTSTP		20	/* interactive stop signal */
#define SIGTTIN		21	/* background process wants to read */
#define SIGTTOU		22	/* background process wants to write */
#define	_NSIG	16
#else
#define	_NSIG	16
#endif	/* __USG or __BSD4_2 */

void	(*signal(int _sig, void (*_func)(int)))(int);
int	raise(int _sig);

#endif	/* _SIGNAL_H */
