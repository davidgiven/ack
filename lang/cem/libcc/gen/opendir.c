/* $Header$ */
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
	if ((unsigned) stbuf.st_size == stbuf.st_size &&
	    (dirp->dd_buf = malloc((unsigned) stbuf.st_size))) {
		dirp->dd_bsize = stbuf.st_size;
		read(fd, dirp->dd_buf, dirp->dd_bsize);
		close(fd);
		dirp->dd_fd = -2;
		dirp->dd_loc = 0;
		return dirp;
	}
	else if (dirp->dd_buf = malloc(8*DIRBLKSIZ)) {
		dirp->dd_bsize = 8 * DIRBLKSIZ;
	}
	else if (dirp->dd_buf = malloc(DIRBLKSIZ)) {
		dirp->dd_bsize =  DIRBLKSIZ;
	}
	else {
		close(fd);
		free((char *) dirp);
		return NULL;
	}
	dirp->dd_fd = fd;
	dirp->dd_loc = -1;
	return dirp;
}
