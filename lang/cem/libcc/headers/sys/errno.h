/*
 * sys/errno.h - error numbers
 */
/* $Id$ */

#if	!defined(_SYS_ERRNO_H)
#define	_SYS_ERRNO_H

#define	EPERM		1	/* Not owner */
#define	ENOENT		2	/* No such file or directory */
#define	ESRCH		3	/* No such process */
#define	EINTR		4	/* Interrupted system call */
#define	EIO		5	/* I/O error */
#define	ENXIO		6	/* No such device or address */
#define	E2BIG		7	/* Arg list too long */
#define	ENOEXEC		8	/* Exec format error */
#define	EBADF		9	/* Bad file number */
#define	ECHILD		10	/* No children */
#define	EAGAIN		11	/* No more processes */
#define	ENOMEM		12	/* Not enough core */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
#define	ENOTBLK		15	/* Block device required */
#define	EBUSY		16	/* Mount device busy */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Cross-device link */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory*/
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* File table overflow */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Not a typewriter */
#define	ETXTBSY		26	/* Text file busy */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Illegal seek */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */

#if	defined(__USG)
/* Only ENOMSG, EIDRM and EDEADLK are documented */
#define	ENOMSG		35	/* No message of desired type */
#define	EIDRM		36	/* Identifier Removed */
#define	ECHRNG		37	/* Channel number out of range */
#define	EL2NSYNC	38	/* Level 2 not synchronized */
#define	EL3HLT		39	/* Level 3 halted */
#define	EL3RST		40	/* Level 3 reset */
#define	ELNRNG		41	/* Link number out of range */
#define	EUNATCH		42	/* Protocol driver not attached */
#define	ENOCSI		43	/* No CSI structure available */
#define	EL2HLT		44	/* Level 2 halted */
#define	EDEADLK		45	/* DeadLock */
#endif	/* __USG */

#if	defined(__BSD4_2)
/* non-blocking and interrupt i/o */
#define	EWOULDBLOCK	35	/* Operation would block */
#define	EINPROGRESS	36	/* Operation now in progress */
#define	EALREADY	37	/* Operation already in progress */
/* ipc/network software */

	/* argument errors */
#define	ENOTSOCK	38	/* Socket operation on non-socket */
#define	EDESTADDRREQ	39	/* Destination address required */
#define	EMSGSIZE	40	/* Message too long */
#define	EPROTOTYPE	41	/* Protocol wrong type for socket */
#define	ENOPROTOOPT	42	/* Protocol not available */
#define	EPROTONOSUPPORT	43	/* Protocol not supported */
#define	ESOCKTNOSUPPORT	44	/* Socket type not supported */
#define	EOPNOTSUPP	45	/* Operation not supported on socket */
#define	EPFNOSUPPORT	46	/* Protocol family not supported */
#define	EAFNOSUPPORT	47	/* Address family not supported by protocol family */
#define	EADDRINUSE	48	/* Address already in use */
#define	EADDRNOTAVAIL	49	/* Can't assign requested address */

	/* operational errors */
#define	ENETDOWN	50	/* Network is down */
#define	ENETUNREACH	51	/* Network is unreachable */
#define	ENETRESET	52	/* Network dropped connection on reset */
#define	ECONNABORTED	53	/* Software caused connection abort */
#define	ECONNRESET	54	/* Connection reset by peer */
#define	ENOBUFS		55	/* No buffer space available */
#define	EISCONN		56	/* Socket is already connected */
#define	ENOTCONN	57	/* Socket is not connected */
#define	ESHUTDOWN	58	/* Can't send after socket shutdown */
/* ETOOMANYREFS is not documented */
#define	ETOOMANYREFS	59	/* Too many references: can't splice */
#define	ETIMEDOUT	60	/* Connection timed out */
#define	ECONNREFUSED	61	/* Connection refused */
 
	/* */
#define	ELOOP		62	/* Too many levels of symbolic links */
#define	ENAMETOOLONG	63	/* File name too long */
 
/* In BSD4.2, ENOTEMPTY is defined as 64. */
/* Just use BSD4.3 & Sun UNIX 4.2 definitions */
#define	EHOSTDOWN	64	/* Host is down */
#define	EHOSTUNREACH	65	/* No route to host */
#define	ENOTEMPTY	66	/* Directory not empty */

/* quotas & mush */
/* EPROCLIM and EUSERS are not documented */
#define	EPROCLIM	67	/* Too many processes */
#define	EUSERS		68	/* Too many users */
#define	EDQUOT		69	/* Disc quota exceeded */

/* Network File System */
#define	ESTALE		70	/* Stale NFS file handle */
#define	EREMOTE		71	/* Too many levels of remote in path */

/* streams */
/* only ENOMSG is documented */
#define	ENOSTR		72	/* Device is not a stream */
#define	ETIME		73	/* Timer expired */
#define	ENOSR		74	/* Out of streams resources */
#define	ENOMSG		75	/* No message of desired type */
#define	EBADMSG		76	/* Trying to read unreadable message */

#define	EIDRM		77	/* Identifier removed */

/* SystemV Record Locking */
#define	EDEADLK		78	/* Deadlock condition. */
#define	ENOLCK		79	/* No record locks available. */

#endif	/* __BSD4_2 */

#if	defined(_POSIX_SOURCE)
#if	defined(_MINIX)
#define	EDEADLK		35	/* resource deadlock avoided */
#define	ENAMETOOLONG	36	/* file name too long */
#define	ENOLCK		37	/* no locks available */
#define	ENOSYS		38	/* function not implemented */
#define	ENOTEMPTY	39	/* directory not empty */

#define	ELOCKED		101	/* can't send message */
#define	EBADCALL	102	/* error on send/recieve */
#define	ELONGSTRING	103	/* string too long */
#endif
/* Room for other POSIX implementations */
#endif

#endif	/* _SYS_ERRNO_H */
