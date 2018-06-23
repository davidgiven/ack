/*
	opendir -- open a directory stream

	last edit:	16-Jun-1987	D A Gwyn
*/

#include <errno.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

typedef void* pointer; /* (void *) if you have it */

extern int _open(const char* path, int flags, int mode);
extern int _close(int d);
extern int _fstat(int fd, struct stat* buf);

#ifndef NULL
#define NULL 0
#endif

#ifndef O_RDONLY
#define O_RDONLY 0
#endif

#ifndef S_ISDIR /* macro to test for directory file */
#define S_ISDIR(mode) (((mode)&S_IFMT) == S_IFDIR)
#endif

DIR* opendir(const char* dirname) /* name of directory */
{
	register DIR* dirp; /* -> malloc'ed storage */
	register int fd; /* file descriptor for read */
	struct stat sbuf; /* result of fstat() */

	if ((fd = _open(dirname, O_RDONLY, 0)) < 0)
		return NULL; /* errno set by open() */

	if (_fstat(fd, &sbuf) != 0 || !S_ISDIR(sbuf.st_mode))
	{
		(void)_close(fd);
		errno = ENOTDIR;
		return NULL; /* not a directory */
	}

	if ((dirp = (DIR*)malloc(sizeof(DIR))) == NULL
	    || (dirp->dd_buf = (char*)malloc((unsigned)DIRBUF)) == NULL)
	{
		register int serrno = errno;
		/* errno set to ENOMEM by sbrk() */

		if (dirp != NULL)
			free((pointer)dirp);

		(void)_close(fd);
		errno = serrno;
		return NULL; /* not enough memory */
	}

	dirp->dd_fd = fd;
	dirp->dd_loc = dirp->dd_size = 0; /* refill needed */

	return dirp;
}
