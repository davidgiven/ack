/* $Id$ */
#include <sys/types.h>
#include <sys/dir.h>

/*
 * close a directory.
 */
closedir(dirp)
register DIR *dirp;
{
	if (dirp->dd_fd >= 0) close(dirp->dd_fd);
	dirp->dd_fd = -1;
	dirp->dd_loc = -1;
	free(dirp->dd_buf);
	free((char *)dirp);
}
