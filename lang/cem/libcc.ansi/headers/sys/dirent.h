/*
	<sys/dirent.h> -- file system independent directory entry (SVR3)

	last edit:	25-Apr-1987	D A Gwyn

	prerequisite:	<sys/types.h>
*/
/* $Id$ */

#if	!defined(_SYS_DIRENT_H)
#define	_SYS_DIRENT_H

struct dirent				/* data from getdents()/readdir() */
	{
	long		d_ino;		/* inode number of entry */
	off_t		d_off;		/* offset of disk directory entry */
	unsigned short	d_reclen;	/* length of this record */
	char		d_name[1];	/* name of file */	/* non-POSIX */
	};

/* The following nonportable ugliness could have been avoided by defining
   DIRENTSIZ and DIRENTBASESIZ to also have (struct dirent *) arguments. */
#define	DIRENTBASESIZ		(((struct dirent *)0)->d_name \
				- (char *)&((struct dirent *)0)->d_ino)
#define	DIRENTSIZ( namlen )	((DIRENTBASESIZ + sizeof(long) + (namlen)) \
				/ sizeof(long) * sizeof(long))

/* DAG -- the following was moved from <dirent.h>, which was the wrong place */
#define	MAXNAMLEN	512		/* maximum filename length */

#if	!defined(NAME_MAX)
#define	NAME_MAX	(MAXNAMLEN - 1)	/* DAG -- added for POSIX */
#endif

#endif	/* _SYS_DIRENT_H */
