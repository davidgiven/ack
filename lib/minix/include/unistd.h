/* The <unistd.h> header contains a few miscellaneous manifest constants. */

#ifndef _UNISTD_H
#define _UNISTD_H

/* Values used by access().  POSIX Table 2-6. */
#define F_OK               0	/* test if file exists */
#define X_OK               1	/* test if file is executable */
#define W_OK               2	/* test if file is writable */
#define R_OK               4	/* test if file is readable */

/* Values used for whence in lseek(fd, offset, whence).  POSIX Table 2-7. */
#define SEEK_SET           0	/* offset is absolute  */
#define SEEK_CUR           1	/* offset is relative to current position */
#define SEEK_END           2	/* offset is relative to end of file */

/* This value is required by POSIX Table 2-8. */
#define _POSIX_VERSION 198808L	/* which standard is being conformed to */

/* These three definitions are required by POSIX Sec. 8.2.1.2. */
#define STDIN_FILENO       0	/* file descriptor for stdin */
#define STDOUT_FILENO      1	/* file descriptor for stdout */
#define STDERR_FILENO      2	/* file descriptor for stderr */

/* NULL must be defined in <unistd.h> according to POSIX Sec. 2.8.1. */
#define NULL    ((void *)0)

/* The following relate to configurable system variables. POSIX Table 4-2. */
#define _SC_ARG_MAX		1
#define _SC_CHILD_MAX		2
#define _SC_CLOCKS_PER_SEC	3
#define _SC_NGROUPS_MAX		4
#define _SC_OPEN_MAX		5
#define _SC_JOB_CONTROL		6
#define _SC_SAVED_IDS		7
#define _SC_VERSION		8

/* The following relate to configurable pathname variables. POSIX Table 5-2. */
#define _PC_LINK_MAX		1	/* link count */
#define _PC_MAX_CANON		2	/* size of the canonical input queue */
#define _PC_MAX_INPUT		3	/* type-ahead buffer size */
#define _PC_NAME_MAX		4	/* file name size */
#define _PC_PATH_MAX		5	/* pathname size */
#define _PC_PIPE_BUF		6	/* pipe size */
#define _PC_NO_TRUNC		7	/* treatment of long name components */
#define _PC_VDISABLE		8	/* tty disable */
#define _PC_CHOWN_RESTRICTED	9	/* chown restricted or not */

/* POSIX defines several options that may be implemented or not, at the
 * implementer's whim.  This implementer has made the following choices:
 *
 * _POSIX_JOB_CONTROL	    not defined:	no job control
 * _POSIX_SAVED_IDS 	    not defined:	no saved uid/gid
 * _POSIX_NO_TRUNC	    not defined:	long path names are truncated
 * _POSIX_CHOWN_RESTRICTED  defined:		you can't give away files
 * _POSIX_VDISABLE	    defined:		tty functions can be disabled
 */
#define _POSIX_CHOWN_RESTRICTED
#define _POSIX_VDISABLE '\t'	/* can't set any control char to tab */


/* Function Prototypes. */
#ifndef _ANSI_H
#include <ansi.h>
#endif

_PROTOTYPE( void _exit, (int _status)					);
_PROTOTYPE( int access, (char *_path, int _amode)			);
_PROTOTYPE( int chdir, (char *_path)					);
_PROTOTYPE( int chown, (char *_path, int _owner, int _group)		);
_PROTOTYPE( int close, (int _fd)					);
_PROTOTYPE( char *ctermid, (char *_s)					);
_PROTOTYPE( char *cuserid, (char *_s)					);
_PROTOTYPE( int dup, (int _fd)						);
_PROTOTYPE( int dup2, (int _fd, int _fd2)				);
_PROTOTYPE( int execl, (char *_path, ...)				);
_PROTOTYPE( int execle, (char *_path, ...)				);
_PROTOTYPE( int execlp, (char *_file, ...)				);
_PROTOTYPE( int execv, (char *_path, char *_argv[])			);
_PROTOTYPE( int execve, (char *_path, char *_argv[], char *_envp[])	);
_PROTOTYPE( int execvp, (char *_file, char *_argv[])			);
_PROTOTYPE( pid_t fork, (void)						);
_PROTOTYPE( long fpathconf, (int _fd, int _name)			);
_PROTOTYPE( char *getcwd, (char *_buf, int _size)			);
_PROTOTYPE( gid_t getegid, (void)					);
_PROTOTYPE( uid_t geteuid, (void)					);
_PROTOTYPE( gid_t getgid, (void)					);
_PROTOTYPE( int getgroups, (int _gidsetsize, gid_t _grouplist[])	);
_PROTOTYPE( char *getlogin, (void)					);
_PROTOTYPE( pid_t getpgrp, (void)					);
_PROTOTYPE( pid_t getpid, (void)					);
_PROTOTYPE( pid_t getppid, (void)					);
_PROTOTYPE( uid_t getuid, (void)					);
_PROTOTYPE( unsigned int alarm, (unsigned int _seconds)			);
_PROTOTYPE( unsigned int sleep, (unsigned int _seconds)			);
_PROTOTYPE( int isatty, (int _fd)					);
_PROTOTYPE( int link, (const char *_path1, const char *_path2)		);
_PROTOTYPE( off_t lseek, (int _fd, off_t _offset, int _whence)		);
_PROTOTYPE( long pathconf, (char *_path, int _name)			);
_PROTOTYPE( int pause, (void)						);
_PROTOTYPE( int pipe, (int _fildes[2])					);
_PROTOTYPE( int read, (int _fd, char *_buf, unsigned int _n)		);
_PROTOTYPE( int rmdir, (const char *_path)				);
_PROTOTYPE( int setgid, (int _gid)					);
_PROTOTYPE( int setpgid, (pid_t _pid, pid_t _pgid)			);
_PROTOTYPE( pid_t setsid, (void)					);
_PROTOTYPE( int setuid, (int _uid)					);
_PROTOTYPE( long sysconf, (int _name)					);
_PROTOTYPE( pid_t tcgetpgrp, (int _fd)					);
_PROTOTYPE( int tcsetpgrp, (int _fd, pid_t _pgrp_id)			);
_PROTOTYPE( char *ttyname, (int _fd)					);
_PROTOTYPE( int unlink, (const char *_path)				);
_PROTOTYPE( int write, (int _fd, char *_buf, unsigned int _n)		);

#ifdef _MINIX
_PROTOTYPE( char *brk, (char *_addr)					);
_PROTOTYPE( int mknod, (const char *_name, int _mode, int _addr)	);
_PROTOTYPE( int mknod4, (const char *_name, int _mode, int _addr,
	    unsigned _size)						);
_PROTOTYPE( char *mktemp, (char *_template)				);
_PROTOTYPE( char *sbrk, (int _incr)					);
_PROTOTYPE( int chroot, (const char *_name)				);
_PROTOTYPE( int mount, (char *_spec, char *_name, int _flag));
_PROTOTYPE( long ptrace, (int _req, int _pid, long _addr, long _data)	);
_PROTOTYPE( int stime, (long *top)					);
_PROTOTYPE( int sync, (void)						);
_PROTOTYPE( int umount, (const char *_name)				);
#endif

#endif /* _UNISTD_H */
