/* $Id$ */
#include <sys/types.h>
#include <sys/dir.h>

/*
 * seek to an entry in a directory.
 * Only values returned by "telldir" should be passed to seekdir.
 */
seekdir(dirp, loc)
register DIR *dirp;
long loc;
{
	long curloc, base, offset;
	extern long telldir();
	extern struct direct *readdir();

	curloc = telldir(dirp);
	if (loc == curloc)
		return;
	offset = loc % dirp->dd_bsize;
	base = loc - offset;
	if (dirp->dd_loc != -1 &&
	    (curloc - (curloc % dirp->dd_bsize)) ==  base) {
		dirp->dd_loc = offset;
		return;
	}
	(void) lseek(dirp->dd_fd, base, 0);
	dirp->dd_loc = -1;
	dirp->dd_size = 0;
	while (dirp->dd_loc < offset) {
		if (readdir(dirp) == (struct direct *) 0)
			return;
	}
}
