/* The <signal.h> header defines all the ANSI and POSIX signals.
 * MINIX supports all the signals required by POSIX. They are defined below.
 * Some additional signals are also supported.
 */

#ifndef _SIGNAL_H
#define _SIGNAL_H

/* Here are types that are closely associated with signal handling. */
typedef int sig_atomic_t;

#ifdef	_POSIX_SOURCE
typedef unsigned short sigset_t;
#endif


#define _NSIG             16	/* number of signals used */

#define SIGHUP	           1	/* hangup */
#define SIGINT             2	/* interrupt (DEL) */
#define SIGQUIT            3	/* quit (ASCII FS) */
#define SIGILL             4	/* illegal instruction */
#define SIGTRAP            5	/* trace trap (not reset when caught) */
#define SIGABRT            6	/* IOT instruction */
#define SIGIOT             6	/* SIGABRT for people who speak PDP-11 */
#define SIGUNUSED          7	/* spare code */
#define SIGFPE             8	/* floating point exception */
#define SIGKILL            9	/* kill (cannot be caught or ignored) */
#define SIGUSR1           10	/* user defined signal # 1 */
#define SIGSEGV           11	/* segmentation violation */
#define SIGUSR2           12	/* user defined signal # 2 */
#define SIGPIPE           13	/* write on a pipe with no one to read it */
#define SIGALRM           14	/* alarm clock */
#define SIGTERM           15	/* software termination signal from kill */
#define SIGSTKFLT         16	/* used by kernel to indicate stack fault */

#define SIGEMT             7	/* obsolete */
#define SIGBUS            10	/* obsolete */

/* POSIX requires the following signals to be defined, even if they are
 * not supported.  Here are the definitions, but they are not supported.
 */
#define SIGCHLD           17	/* child process terminated or stopped */
#define SIGCONT           18	/* continue if stopped */
#define SIGSTOP           19	/* stop signal */
#define SIGTSTP           20	/* interactive stop signal */
#define SIGTTIN           21	/* background process wants to read */
#define SIGTTOU           22	/* background process wants to write */

#ifdef _POSIX_SOURCE
#define SA_NOCLDSTOP       1	/* signal parent if child stops */

#endif /* _POSIX_SOURCE */

/* POSIX requires these values for use on system calls involving signals. */
#define SIG_BLOCK          0	/* for blocking signals */
#define SIG_UNBLOCK        1	/* for unblocking signals */
#define SIG_SETMASK        2	/* for setting the signal mask */

#ifndef _ANSI_H
#include <ansi.h>
#endif

/* Macros used as function pointers and one awful prototype. */
#if _ANSI
#define SIG_DFL		((void (*)(int))0)	/* default signal handling */
#define SIG_IGN		((void (*)(int))1)	/* ignore signal */
#define SIG_ERR		((void (*)(int))-1)

void (*signal(int _sig, void (*_func)(int)))(int);

#ifdef _POSIX_SOURCE
struct sigaction {
  void (*sa_handler)(int);	/* SIG_DFL, SIG_IGN, or pointer to function */
  sigset_t sa_mask;		/* signals to be blocked during handler */
  int sa_flags;			/* special flags */
};
#endif

#else	/* !_ANSI */
#define SIG_DFL		((void (*)())0)		/* default signal handling */
#define SIG_IGN		((void (*)())1)		/* ignore signal */
#define SIG_ERR		((void (*)())-1)

void (*signal()) ();

#ifdef _POSIX_SOURCE		/* otherwise sigset_t is not defined */
struct sigaction {
  void (*sa_handler)();		/* SIG_DFL, SIG_IGN, or pointer to function */
  sigset_t sa_mask;		/* signals to be blocked during handler */
  int sa_flags;			/* special flags */
};
#endif

#endif	/* _ANSI */

/* Function Prototypes. */
_PROTOTYPE( int raise, (int _sig)					);

#ifdef _POSIX_SOURCE
_PROTOTYPE( int kill, (pid_t _pid, int _sig)				);
_PROTOTYPE( int sigaddset, (sigset_t *_set)				);
_PROTOTYPE( int sigdelset, (sigset_t *_set)				);
_PROTOTYPE( int sigemptyset, (sigset_t *_set)				);
_PROTOTYPE( int sigfillset, (sigset_t *_set)				);
_PROTOTYPE( int sigismember, (sigset_t *_set, int _signo)		);
_PROTOTYPE( int sigpending, (sigset_t *set)				);
_PROTOTYPE( int sigprocmask, (int _how, sigset_t *_set, sigset_t *_oset));
_PROTOTYPE( int sigsuspend, (sigset_t *_sigmask)			);
_PROTOTYPE( int sigaction,
	    (int _sig, struct sigaction *_a, struct sigaction *_oact)	);
#endif

#endif /* _SIGNAL_H */
