/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef _STAT_H
#define _STAT_H

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
#ifdef __BSD4_2
	long	st_spare1;
#endif
	time_t	st_mtime;
#ifdef __BSD4_2
	long	st_spare2;
#endif
	time_t	st_ctime;
#ifdef __BSD4_2
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
#ifndef __BSD4_2
#ifndef __USG
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

#endif /* _STAT_H */
