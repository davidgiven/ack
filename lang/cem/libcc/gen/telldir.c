#include <sys/types.h>
#include <sys/dir.h>

/*
 * return a pointer into a directory
 */
long telldir(dirp)
DIR *dirp;
{
	extern long lseek();

	return (lseek(dirp->dd_fd, 0L, 1) - dirp->dd_size + dirp->dd_loc);
}
