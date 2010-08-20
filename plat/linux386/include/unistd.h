/*
 * unistd.h - standard system calls
 */
/* $Id$ */

#ifndef _UNISTD_H
#define _UNISTD_H

#include <stddef.h>
#include <time.h>

/* Types */

typedef int pid_t;
typedef int mode_t;

typedef long suseconds_t;

/* Time handling. */

struct timeval
{
	time_t tv_sec;
	suseconds_t tv_usec;
};

struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
}; /* obsolete, unused */

extern int gettimeofday(struct timeval* tv, struct timezone* tz);
extern int settimeofday(const struct timeval* tv, const struct timezone* tz);

/* File access. */

enum
{
	O_ACCMODE = 0x3,
	
	O_RDONLY = 0,
	O_WRONLY = 1,
	O_RDWR = 2,
	
	O_CREAT = 0x40,
	O_TRUNC = 0x200,
	O_APPEND = 0x400
};

extern int open(const char* path, int access, ...);
extern int creat(const char* path, mode_t mode);
extern int close(int d);
extern int read(int fd, void* buffer, size_t count);
extern int write(int fd, void* buffer, size_t count);
extern off_t lseek(int fildes, off_t offset, int whence);
extern int fcntl(int fd, int op, ...);

/* Special variables */

extern char** environ;

/* Implemented system calls */

extern void _exit(int);
extern pid_t getpid(void);
extern int brk(void* ptr);
extern void* sbrk(intptr_t increment);
extern int isatty(int d);

/* Signal handling */

typedef int sig_atomic_t;

#define SIG_ERR ((sighandler_t) -1)           /* Error return.  */
#define SIG_DFL ((sighandler_t) 0)            /* Default action.  */
#define SIG_IGN ((sighandler_t) 1)            /* Ignore signal.  */

#define SIGHUP          1       /* Hangup (POSIX).  */
#define SIGINT          2       /* Interrupt (ANSI).  */
#define SIGQUIT         3       /* Quit (POSIX).  */
#define SIGILL          4       /* Illegal instruction (ANSI).  */
#define SIGTRAP         5       /* Trace trap (POSIX).  */
#define SIGABRT         6       /* Abort (ANSI).  */
#define SIGIOT          6       /* IOT trap (4.2 BSD).  */
#define SIGBUS          7       /* BUS error (4.2 BSD).  */
#define SIGFPE          8       /* Floating-point exception (ANSI).  */
#define SIGKILL         9       /* Kill, unblockable (POSIX).  */
#define SIGUSR1         10      /* User-defined signal 1 (POSIX).  */
#define SIGSEGV         11      /* Segmentation violation (ANSI).  */
#define SIGUSR2         12      /* User-defined signal 2 (POSIX).  */
#define SIGPIPE         13      /* Broken pipe (POSIX).  */
#define SIGALRM         14      /* Alarm clock (POSIX).  */
#define SIGTERM         15      /* Termination (ANSI).  */
#define SIGSTKFLT       16      /* Stack fault.  */
#define SIGCLD          SIGCHLD /* Same as SIGCHLD (System V).  */
#define SIGCHLD         17      /* Child status has changed (POSIX).  */
#define SIGCONT         18      /* Continue (POSIX).  */
#define SIGSTOP         19      /* Stop, unblockable (POSIX).  */
#define SIGTSTP         20      /* Keyboard stop (POSIX).  */
#define SIGTTIN         21      /* Background read from tty (POSIX).  */
#define SIGTTOU         22      /* Background write to tty (POSIX).  */
#define SIGURG          23      /* Urgent condition on socket (4.2 BSD).  */
#define SIGXCPU         24      /* CPU limit exceeded (4.2 BSD).  */
#define SIGXFSZ         25      /* File size limit exceeded (4.2 BSD).  */
#define SIGVTALRM       26      /* Virtual alarm clock (4.2 BSD).  */
#define SIGPROF         27      /* Profiling alarm clock (4.2 BSD).  */
#define SIGWINCH        28      /* Window size change (4.3 BSD, Sun).  */
#define SIGPOLL         SIGIO   /* Pollable event occurred (System V).  */
#define SIGIO           29      /* I/O now possible (4.2 BSD).  */
#define SIGPWR          30      /* Power failure restart (System V).  */
#define SIGSYS          31      /* Bad system call.  */
#define SIGUNUSED       31

#define _NSIG           32      /* Biggest signal number + 1
                                   (not including real-time signals).  */
typedef void (*sighandler_t)(int);
extern sighandler_t signal(int signum, sighandler_t handler);
extern int raise(int signum);



#endif
