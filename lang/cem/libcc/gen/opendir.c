/* $Id$ */
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
	long siz;
	extern char *malloc();

#ifdef __BSD4_2
	siz = stbuf.st_blksize;
#else
	siz = DIRBLKSIZ;
#endif
	if ((fd = open(name, 0)) == -1)
		return NULL;
	fstat(fd, &stbuf);
	if (((stbuf.st_mode & S_IFDIR) == 0) ||
	    ((dirp = (DIR *)malloc(sizeof (DIR))) == NULL)) {
		close (fd);
		return NULL;
	}
	if (stbuf.st_size > siz) siz = stbuf.st_size;
	if ((unsigned) siz == siz &&
	    (dirp->dd_buf = malloc((unsigned) siz))) {
		dirp->dd_bsize = siz;
#ifdef __BSD4_2
		dirp->dd_size = getdirentries(fd,
					      (char *) dirp->dd_buf,
					      (int) siz,
					      &siz);
		if (dirp->dd_size < 0 )
#endif
		dirp->dd_size = read(fd, dirp->dd_buf, dirp->dd_bsize);
		close(fd);
		dirp->dd_fd = -2;
		dirp->dd_loc = 0;
		return dirp;
	}
#ifndef __BSD4_2
	else if (dirp->dd_buf = malloc(8*DIRBLKSIZ)) {
		dirp->dd_bsize = 8 * DIRBLKSIZ;
	}
	else if (dirp->dd_buf = malloc(DIRBLKSIZ)) {
		dirp->dd_bsize =  DIRBLKSIZ;
	}
#endif
	else {
		close(fd);
		free((char *) dirp);
		return NULL;
	}
	dirp->dd_fd = fd;
	dirp->dd_loc = -1;
	return dirp;
}
