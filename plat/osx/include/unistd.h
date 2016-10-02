#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h>

/*
 * XXX - The following parts belong in other header files,
 * but those headers are including us!
 */

/* XXX - begin sys/ioctl.h */

#define TIOCGETD	0x4004741a

int ioctl(int, unsigned long, ...);

/* XXX - end sys/ioctl.h */

/* XXX - begin sys/time.h */

/* Don't conflict with time_t from <time.h> */
typedef long _libsys_time_t;
typedef int suseconds_t;

struct timeval {
	_libsys_time_t tv_sec;
	suseconds_t tv_usec;
};

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

int gettimeofday(struct timeval *, struct timezone *);

/* XXX - end sys/time.h */

/* XXX - begin fcntl.h */

/* flags for open() */
#define O_RDONLY	0x0000
#define O_WRONLY	0x0001
#define O_RDWR		0x0002
#define O_NONBLOCK	0x0004
#define O_APPEND	0x0008
#define O_CREAT		0x0200
#define O_TRUNC		0x0400
#define O_EXCL		0x0800

typedef int mode_t;

int creat(const char *, mode_t);
int open(const char *, int, ...);

/* XXX - end fcntl.h */

/* XXX - begin signal.h */

#define SIGHUP		1
#define SIGINT		2
#define SIGQUIT		3
#define SIGILL		4
#define SIGTRAP		5
#define SIGABRT		6
#define SIGEMT		7
#define SIGFPE		8
#define SIGKILL		9
#define SIGBUS		10
#define SIGSEGV		11
#define SIGSYS		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGURG		16
#define SIGSTOP		17
#define SIGTSTP		18
#define SIGCONT		19
#define SIGCHLD		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGIO		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGINFO		29
#define SIGUSR1		30
#define SIGUSR2		31
#define _NSIG		32

/* sa_flags */
#define SA_RESTART	0x0002

typedef void (*sig_t)(int);
#define SIG_DFL ((sig_t)0)
#define SIG_IGN ((sig_t)1)
#define SIG_ERR ((sig_t)-1)

typedef unsigned int sigset_t;

struct __siginfo;

struct sigaction {
	union {
		void (*__sa_handler)(int);
		void (*__sa_sigaction)(int, struct __siginfo *, void *);
	} __sigaction_u;
	sigset_t sa_mask;
	int sa_flags;
};
#define sa_handler __sigaction_u.__sa_handler
#define sa_sigaction __sigaction_u.__sa_sigaction

int kill(pid_t, int);
int sigaction(int, const struct sigaction *, struct sigaction *);
sig_t signal(int, sig_t);

/* XXX - end signal.h */

void _exit(int);
int brk(void *);
int close(int);
pid_t getpid(void);
int isatty(int);
off_t lseek(int, off_t, int);
ssize_t read(int, void *, size_t);
void *sbrk(int);
ssize_t write(int, const void *, size_t);

#endif
