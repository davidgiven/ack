/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef _SIGNAL_H
#define _SIGNAL_H

#ifdef __BSD4_2
#define _NSIG             32	/* number of signals used */
#else
#define _NSIG             16	/* number of signals used */
#endif
#define NSIG _NSIG

#define SIGHUP	           1	/* hangup */
#define SIGINT             2	/* interrupt (DEL) */
#define SIGQUIT            3	/* quit (ASCII FS) */
#define SIGILL             4	/* illegal instruction */
#define SIGTRAP            5	/* trace trap (not reset when caught) */
#define SIGABRT            6	/* IOT instruction */
#define SIGIOT             6	/* SIGABRT for people who speak PDP-11 */
#define SIGEMT             7	/* EMT instruction */
#define SIGFPE             8	/* floating point exception */
#define SIGKILL            9	/* kill (cannot be caught or ignored) */
#define SIGBUS            10	/* bus error */
#define SIGSEGV           11	/* segmentation violation */
#define SIGSYS            12	/* bad argument to system call */
#define SIGPIPE           13	/* write on a pipe with no one to read it */
#define SIGALRM           14	/* alarm clock */
#define SIGTERM           15	/* software termination signal from kill */

#ifdef __BSD4_2
#define SIGURG            16    /* urgent condition on IO channel */
#define SIGCHLD           20	/* child process terminated or stopped */
#define SIGCONT           19	/* continue if stopped */
#define SIGSTOP           17	/* stop signal */
#define SIGTSTP           18	/* interactive stop signal */
#define SIGTTIN           21	/* background process wants to read */
#define SIGTTOU           22	/* background process wants to write */
#define SIGIO             23    /* input/output possible signal */
#define SIGPOLL        SIGIO    /* System V name for SIGIO */
#define SIGXCPU           24    /* exceeded CPU time limit */
#define SIGXFSZ           25    /* exceeded file size limit */
#define SIGVTALRM         26    /* virtual time alarm */
#define SIGPROF           27    /* profiling time alarm */
#define SIGWINCH          28    /* window changed */
#define SIGLOST           29    /* resource lost (eg, record-lock lost) */
#define SIGUSR1           30    /* user defined signal 1 */
#define SIGUSR2           31    /* user defined signal 2 */
#endif


#define SIG_DFL		((int (*)())0)		/* default signal handling */
#define SIG_IGN		((int (*)())1)		/* ignore signal */
#define SIG_ERR		((int (*)())-1)

int (*signal()) ();

#endif /* _SIGNAL_H */
