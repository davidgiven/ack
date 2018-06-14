/*
 * unistd.h - standard system calls
 */

#ifndef _UNISTD_H
#define _UNISTD_H

#include <stddef.h>

/* Types */

typedef int pid_t;
typedef int mode_t;

/* Constants for file access (open and friends) */

enum
{
	O_ACCMODE = 0x3,
	
	O_RDONLY = 0,
	O_WRONLY = 1,
	O_RDWR = 2,
	
	O_CREAT = 0100,
	O_TRUNC = 01000,
	O_APPEND = 02000,
	O_NONBLOCK = 04000
};

/* Special variables */

extern char** environ;

/* Implemented system calls */

extern void _exit(int);
extern pid_t getpid(void);
extern int brk(void* addr);
extern void* sbrk(int increment);
extern int isatty(int d);
extern off_t lseek(int fildes, off_t offset, int whence);
extern int close(int d);
extern int open(const char* path, int access, ...);
extern int creat(const char* path, mode_t mode);
extern int read(int fd, void* buffer, size_t count);
extern int write(int fd, void* buffer, size_t count);

/* Unimplemented system calls (these are just prototypes to let the library
 * compile). */

extern int fcntl(int fd, int op, ...);

/* Signal handling */

typedef int sig_atomic_t;

#define SIG_ERR ((sighandler_t) -1)           /* Error return.  */
#define SIG_DFL ((sighandler_t) 0)            /* Default action.  */
#define SIG_IGN ((sighandler_t) 1)            /* Ignore signal.  */

#define _NSIG             16	/* number of signals used */

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

typedef void (*sighandler_t)(int);
extern sighandler_t signal(int signum, sighandler_t handler);
extern int raise(int signum);

#endif
