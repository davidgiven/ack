/*
	rewinddir -- rewind a directory stream

	last edit:	25-Apr-1987	D A Gwyn

	This is not simply a call to seekdir(), because seekdir()
	will use the current buffer whenever possible and we need
	rewinddir() to forget about buffered data.
*/

#include <errno.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <dirent.h>

extern off_t _lseek(int d, int offset, int whence);

#ifndef NULL
#define NULL 0
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

void rewinddir(register DIR* dirp)
{
	if (dirp == NULL || dirp->dd_buf == NULL)
	{
		errno = EFAULT;
		return; /* invalid pointer */
	}

	dirp->dd_loc = dirp->dd_size = 0; /* invalidate buffer */
	(void)_lseek(dirp->dd_fd, (off_t)0, SEEK_SET); /* may set errno */
}
