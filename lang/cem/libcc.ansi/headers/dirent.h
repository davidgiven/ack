/*
	<dirent.h> -- definitions for SVR3 directory access routines

	last edit:	25-Apr-1987	D A Gwyn

	Prerequisite:	<sys/types.h>
*/
/* $Header$ */

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

DIR *opendir(const char *__dirname);
struct dirent *readdir(DIR *__dirp);
void rewinddir(DIR *__dirp);
int closedir(DIR *__dirp);
off_t telldir(DIR *__dirp);
void seekdir(DIR *__dirp, off_t __loc);

#define	NULL		((void *)0)	/* DAG -- added for convenience */

#endif	/* _DIRENT_H */
