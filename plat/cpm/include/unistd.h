/*
 * unistd.h - standard system calls
 */
/* $Id$ */

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

#define SIGABRT         6       /* Abort (ANSI) */
#define SIGILL          11      /* Illegal instruction */

#define _NSIG           32      /* Biggest signal number + 1
                                   (not including real-time signals).  */
typedef void (*sighandler_t)(int);
extern sighandler_t signal(int signum, sighandler_t handler);
extern int raise(int signum);

#endif
