#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>

/*
 * open a directory.
 */
DIR *opendir(name)
char *name;
{
	register DIR *dirp;
	register int fd;
	struct stat stbuf;
	extern char *malloc();

	if ((fd = open(name, 0)) == -1)
		return NULL;
	fstat(fd, &stbuf);
	if (((stbuf.st_mode & S_IFDIR) == 0) ||
	    ((dirp = (DIR *)malloc(sizeof (DIR))) == NULL)) {
		close (fd);
		return NULL;
	}
	dirp->dd_fd = fd;
	dirp->dd_loc = 0;
	return dirp;
}
