/*
	readdir -- read next entry from a directory stream

	last edit:	25-Apr-1987	D A Gwyn
*/

#include <errno.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <dirent.h>

/* SVR3 system call, or emulation for getdents() */
extern int getdents(int fildes, char* buf, unsigned nbyte);

#ifndef NULL
#define NULL 0
#endif

struct dirent*
readdir(register DIR* dirp)
{
	register struct dirent* dp; /* -> directory data */

	if (dirp == NULL || dirp->dd_buf == NULL)
	{
		errno = EFAULT;
		return NULL; /* invalid pointer */
	}

	do
	{
		if (dirp->dd_loc >= dirp->dd_size) /* empty or obsolete */
			dirp->dd_loc = dirp->dd_size = 0;

		if (dirp->dd_size == 0 /* need to refill buffer */
		    && (dirp->dd_size = getdents(dirp->dd_fd, dirp->dd_buf, (unsigned)DIRBUF)) <= 0)
			return NULL; /* EOF or error */

		dp = (struct dirent*)&dirp->dd_buf[dirp->dd_loc];
		dirp->dd_loc += dp->d_reclen;
	} while (dp->d_ino == 0L); /* don't rely on getdents() */

	return dp;
}
