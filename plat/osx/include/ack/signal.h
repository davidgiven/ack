#ifndef _ACK_SIGNAL_H
#define _ACK_SIGNAL_H

#define SIGHUP          1
#define SIGINT          2
#define SIGQUIT         3
#define SIGILL          4
#define SIGTRAP         5
#define SIGABRT         6
#define SIGEMT          7
#define SIGFPE          8
#define SIGKILL         9
#define SIGBUS          10
#define SIGSEGV         11
#define SIGSYS          12
#define SIGPIPE         13
#define SIGALRM         14
#define SIGTERM         15
#define SIGURG          16
#define SIGSTOP         17
#define SIGTSTP         18
#define SIGCONT         19
#define SIGCHLD         20
#define SIGTTIN         21
#define SIGTTOU         22
#define SIGIO           23
#define SIGXCPU         24
#define SIGXFSZ         25
#define SIGVTALRM       26
#define SIGPROF         27
#define SIGWINCH        28
#define SIGINFO         29
#define SIGUSR1         30
#define SIGUSR2         31
#define _NSIG           32

/* sa_flags */
#define SA_RESTART      0x0002

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

#endif
