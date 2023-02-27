#ifndef _UNISTD_H
#define _UNISTD_H

/* 
 * Generic Posix prototypes used by the rest of the libc. Plats don't have to
 * implement these if they don't care about the functionality, but if they do,
 * they must conform to these prototypes.
 */

#include <ack/config.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#if ACKCONF_WANT_STANDARD_O
    /* A standard set of flags for read/write/creat and friends for platforms
    * where these aren't determined by the operating system. */

    enum
    {
        O_ACCMODE = 0x3,
        
        O_RDONLY = 0,
        O_WRONLY = 1,
        O_RDWR = 2,
        
    #if ACKCONF_WANT_O_TEXT_O_BINARY
        O_TEXT = 010000,
        O_BINARY = 020000,
    #endif
        O_CREAT = 0100,
        O_TRUNC = 01000,
        O_APPEND = 02000,
        O_NONBLOCK = 04000
    };
#else
    #include <ack/fcntl.h>
#endif

#if ACKCONF_WANT_STANDARD_SIGNALS
    /* A standard set of definitions for signal handling for platforms where these
    * aren't determined by the operating system. */

    typedef int sig_atomic_t;

    #define SIG_ERR ((sighandler_t) -1)           /* Error return.  */
    #define SIG_DFL ((sighandler_t) 0)            /* Default action.  */
    #define SIG_IGN ((sighandler_t) 1)            /* Ignore signal.  */

    #define SIGINT          2       /* Terminal interrupt */
    #define SIGQUIT         3       /* Terminal quit */
    #define SIGABRT         6       /* Abort (ANSI) */
    #define SIGILL          11      /* Illegal instruction */
    #define SIGALRM         14      /* Alarm clock */

    #define _NSIG           16      /* Biggest signal number + 1
                                    (not including real-time signals).  */
    typedef uint16_t sigset_t;

    struct sigaction;
#else
    #include <ack/signal.h>
#endif
typedef void (*sighandler_t)(int);

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

/* Special variables */

extern char** environ;
extern char* optarg;
extern int opterr, optind, optopt;

/* Implemented system calls */

extern int access(const char* pathname, int mode);
extern int brk(void* ptr);
extern int close(int d);
extern int creat(const char* path, mode_t mode);
extern int dup(int oldfd);
extern int dup2(int oldfd, int newfd);
extern int execl(const char *path, const char* arg, ...);
extern int execve(const char *path, char *const argv[], char *const envp[]);
extern int fcntl(int fd, int op, ...);
extern int getopt(int argc, char *const argv[], const char* opts);
extern int gettimeofday(struct timeval* tv, struct timezone* tz);
extern int isatty(int d);
extern int isatty(int d);
extern int kill(pid_t old, int sig);
extern int open(const char* path, int access, ...);
extern int pause(void);
extern int pipe(int pipefd[2]);
extern int raise(int signum);
extern int settimeofday(const struct timeval* tv, const struct timezone* tz);
extern int sigaction(int, const struct sigaction *, struct sigaction *);
extern int sigprocmask(int, const sigset_t *, sigset_t *);
extern int unlink(const char* path);
extern off_t lseek(int fildes, off_t offset, int whence);
extern pid_t fork(void);
extern pid_t getpid(void);
extern unsigned int alarm(unsigned int seconds);
extern pid_t wait(int* wstatus);
extern sighandler_t signal(int signum, sighandler_t handler);
extern ssize_t read(int fd, void* buffer, size_t count);
extern ssize_t write(int fd, void* buffer, size_t count);
extern void _exit(int);
extern void* sbrk(int increment);

#endif
