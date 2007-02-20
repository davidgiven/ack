/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#if	!defined(_SYS_STAT_H)
#define	_SYS_STAT_H

struct	stat {
	dev_t	st_dev;
	ino_t	st_ino;
	u_short st_mode;
	short	st_nlink;
	short	st_uid;
	short	st_gid;
	dev_t	st_rdev;
	off_t	st_size;
	time_t	st_atime;
#if	defined(__BSD4_2)
	long	st_spare1;
#endif
	time_t	st_mtime;
#if	defined(__BSD4_2)
	long	st_spare2;
#endif
	time_t	st_ctime;
#if	defined(__BSD4_2)
	long	st_spare3;
	long	st_blksize;
	long	st_blocks;
	long	st_spare4[2];
#endif
};

#define	S_IFMT		0170000
#define   S_IFDIR	0040000
#define	  S_IFCHR	0020000
#define	  S_IFBLK	0060000
#define	  S_IFREG	0100000
#if	!defined(__BSD4_2)
#if	!defined(__USG) && !defined(_POSIX_SOURCE)
#define	  S_IFMPC	0030000
#define	  S_IFMPB	0070000
#else
#define	  S_IFIFO	0010000
#endif
#else
#define	  S_IFLNK	0120000
#define	  S_IFSOCK	0140000
#endif
#define	S_ISUID		0004000
#define	S_ISGID		0002000
#define	S_ISVTX		0001000
#define	S_IREAD		0000400
#define	S_IWRITE	0000200
#define	S_IEXEC		0000100

#if	defined(__BSD4_2) || defined(_POSIX_SOURCE)
#define	S_IRWXU		00700		/* owner:  rwx------ */
#define	S_IRUSR		00400		/* owner:  r-------- */
#define	S_IWUSR		00200		/* owner:  -w------- */
#define	S_IXUSR		00100		/* owner:  --x------ */
 
#define	S_IRWXG		00070		/* group:  ---rwx--- */
#define	S_IRGRP		00040		/* group:  ---r----- */
#define	S_IWGRP		00020		/* group:  ----w---- */
#define	S_IXGRP		00010		/* group:  -----x--- */
 
#define	S_IRWXO		00007		/* others: ------rwx */
#define	S_IROTH		00004		/* others: ------r-- */ 
#define	S_IWOTH		00002		/* others: -------w- */
#define	S_IXOTH		00001		/* others: --------x */
 
#if	defined(_POSIX_SOURCE)
/* The following macros test st_mode (from POSIX Sec. 5.6.1.1.) */
#define	S_ISREG(m)	((m & S_IFMT) == S_IFREG)	/* is a reg file */
#define	S_ISDIR(m)	((m & S_IFMT) == S_IFDIR)	/* is a directory */
#define	S_ISCHR(m)	((m & S_IFMT) == S_IFCHR)	/* is a char spec */
#define	S_ISBLK(m)	((m & S_IFMT) == S_IFBLK)	/* is a block spec */
#define	S_ISFIFO(m)	((m & S_IFMT) == S_IFIFO)	/* is a pipe/FIFO */
#endif	/* _POSIX_SOURCE */
#endif	/* BSD4_2 || _POSIX_SOURCE */

#endif	/* _SYS_STAT_H */
