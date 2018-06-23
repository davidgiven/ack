/*
	telldir -- report directory stream position

	last edit:	25-Apr-1987	D A Gwyn

	NOTE:	4.nBSD directory compaction makes seekdir() & telldir()
		practically impossible to do right.  Avoid using them!
*/

#include <errno.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <dirent.h>

extern off_t _lseek(int d, int offset, int whence);

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

off_t telldir(register DIR* dirp) /* return offset of next entry */
{
	if (dirp == NULL || dirp->dd_buf == NULL)
	{
		errno = EFAULT;
		return -1; /* invalid pointer */
	}

	if (dirp->dd_loc < dirp->dd_size) /* valid index */
		return ((struct dirent*)&dirp->dd_buf[dirp->dd_loc])->d_off;
	else /* beginning of next directory block */
		return _lseek(dirp->dd_fd, (off_t)0, SEEK_CUR);
}
