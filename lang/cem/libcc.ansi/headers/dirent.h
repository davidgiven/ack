/*
	<dirent.h> -- definitions for SVR3 directory access routines

	last edit:	25-Apr-1987	D A Gwyn

	Prerequisite:	<sys/types.h>
*/
/* $Id$ */

#if	!defined(_DIRENT_H)
#define	_DIRENT_H
#include	<sys/dirent.h>

#define	DIRBUF		8192		/* buffer size for fs-indep. dirs */
	/* must in general be larger than the filesystem buffer size */

typedef struct
	{
	int	dd_fd;			/* file descriptor */
	int	dd_loc;			/* offset in block */
	int	dd_size;		/* amount of valid data */
	char	*dd_buf;		/* -> directory block */
	}	DIR;			/* stream data from opendir() */

DIR *opendir(const char *_dirname);
struct dirent *readdir(DIR *_dirp);
void rewinddir(DIR *_dirp);
int closedir(DIR *_dirp);
off_t telldir(DIR *_dirp);
void seekdir(DIR *_dirp, off_t _loc);

#define	NULL		((void *)0)	/* DAG -- added for convenience */

#endif	/* _DIRENT_H */
