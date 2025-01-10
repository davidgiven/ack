/* The <fcntl.h> header is needed by the open() and fcntl() system calls,
 * which  have a variety of parameters and flags.  They are described here.
 * The formats of the calls to each of these are:
 *
 *	open(path, oflag [,mode])	open a file
 *	fcntl(fd, cmd [,arg])		get or set file attributes
 *
 */

#ifndef _ACK_FCNTL_H
#define _ACK_FCNTL_H

#include <sys/types.h>

/* These values are used for cmd in fcntl().  POSIX Table 6-1.  */
#define F_DUPFD  0 /* duplicate file descriptor */
#define F_GETFD  1 /* get file descriptor flags */
#define F_SETFD  2 /* set file descriptor flags */
#define F_GETFL  3 /* get file status flags */
#define F_SETFL  4 /* set file status flags */
#define F_GETLK  5 /* get record locking information */
#define F_SETLK  6 /* set record locking information */
#define F_SETLKW 7 /* set record locking info; wait if blocked */

/* File descriptor flags used for fcntl().  POSIX Table 6-2. */
#define FD_CLOEXEC 1 /* close on exec flag for third arg of fcntl */

/* L_type values for record locking with fcntl().  POSIX Table 6-3. */
#define F_RDLCK 0 /* shared or read lock */
#define F_WRLCK 1 /* exclusive or write lock */
#define F_UNLCK 2 /* unlock */

/* Oflag values for open().  POSIX Table 6-4. */
#define O_CREAT  00100 /* creat file if it doesn't exist */
#define O_EXCL   00200 /* exclusive use flag */
#define O_NOCTTY 00400 /* do not assign a controlling terminal */
#define O_TRUNC  01000 /* truncate flag */

/* File status flags for open() and fcntl().  POSIX Table 6-5. */
#define O_APPEND   02000 /* set append mode */
#define O_NONBLOCK 04000 /* no delay */

/* File access modes for open() and fcntl().  POSIX Table 6-6. */
#define O_RDONLY 0 /* open(name, O_RDONLY) opens read only */
#define O_WRONLY 1 /* open(name, O_WRONLY) opens write only */
#define O_RDWR   2 /* open(name, O_RDWR) opens read/write */

/* Mask for use with file access modes.  POSIX Table 6-7. */
#define O_ACCMODE 03 /* mask for file access modes */

/* Struct used for locking.  POSIX Table 6-8. */
struct flock
{
	short l_type; /* type: F_RDLCK, F_WRLCK, or F_UNLCK */
	short l_whence; /* flag for starting offset */
	off_t l_start; /* relative offset in bytes */
	off_t l_len; /* size; if 0, then until EOF */
	pid_t l_pid; /* process id of the locks' owner */
};

/* The following relate to configurable system variables. POSIX Table 4-2. */
#define _SC_ARG_MAX        1
#define _SC_CHILD_MAX      2
#define _SC_CLOCKS_PER_SEC 3
#define _SC_NGROUPS_MAX    4
#define _SC_OPEN_MAX       5
#define _SC_JOB_CONTROL    6
#define _SC_SAVED_IDS      7
#define _SC_VERSION        8

/* The following relate to configurable pathname variables. POSIX Table 5-2. */
#define _PC_LINK_MAX         1 /* link count */
#define _PC_MAX_CANON        2 /* size of the canonical input queue */
#define _PC_MAX_INPUT        3 /* type-ahead buffer size */
#define _PC_NAME_MAX         4 /* file name size */
#define _PC_PATH_MAX         5 /* pathname size */
#define _PC_PIPE_BUF         6 /* pipe size */
#define _PC_NO_TRUNC         7 /* treatment of long name components */
#define _PC_VDISABLE         8 /* tty disable */
#define _PC_CHOWN_RESTRICTED 9 /* chown restricted or not */

/* POSIX defines several options that may be implemented or not, at the
 * implementer's whim.  This implementer has made the following choices:
 *
 * _POSIX_JOB_CONTROL       not defined:        no job control
 * _POSIX_SAVED_IDS         not defined:        no saved uid/gid
 * _POSIX_NO_TRUNC          not defined:        long path names are truncated
 * _POSIX_CHOWN_RESTRICTED  defined:            you can't give away files
 * _POSIX_VDISABLE          defined:            tty functions can be disabled
 */
#define _POSIX_CHOWN_RESTRICTED
#define _POSIX_VDISABLE '\t' /* can't set any control char to tab */

extern long fpathconf(int fd, int name);
extern long pathconf(const char* path, int name);

#endif /* _FCNTL_H */
