/* $Id$ */
#include <sys/types.h>
#include <sys/dir.h>

#ifndef __BSD4_2
/*
 * read an old stlye directory entry and present it as a new one
 */
#define	ODIRSIZ	14

struct	olddirect {
	ino_t	od_ino;
	char	od_name[ODIRSIZ];
};
#else
#define olddirect direct
#endif

/*
 * get next entry in a directory.
 */
struct direct *readdir(dirp)
register DIR *dirp;
{
	register struct olddirect *dp;
	static struct direct dir;

	for (;;) {
		if (dirp->dd_loc == -1) {
			dirp->dd_size = read(dirp->dd_fd, dirp->dd_buf, 
					     dirp->dd_bsize);
			if (dirp->dd_size <= 0) {
				dirp->dd_size = 0;
				return NULL;
			}
			dirp->dd_loc = 0;
#ifdef __BSD4_2
			if (! ((struct direct *) dirp->dd_buf)->d_ino) {
				dirp->dd_loc = ((struct direct *)dirp->dd_buf)->d_reclen;
			}
#endif
		}
		if (dirp->dd_loc >= dirp->dd_size) {
			dirp->dd_loc = -1;
			continue;
		}
		dp = (struct olddirect *) (dirp->dd_buf + dirp->dd_loc);
#ifndef __BSD4_2
		dirp->dd_loc += sizeof (struct olddirect);
		if (dp->od_ino == 0)
			continue;
		dir.d_ino = dp->od_ino;
		strncpy(dir.d_name, dp->od_name, ODIRSIZ);
		dir.d_name[ODIRSIZ] = '\0'; /* insure null termination */
		dir.d_reclen = DIRSIZ(&dir);
		dir.d_namlen = strlen(dir.d_name);
#else
		dirp->dd_loc += dp->d_reclen;
		dir.d_ino = dp->d_ino;
		strcpy(dir.d_name, dp->d_name);
		dir.d_reclen = dp->d_reclen;
		dir.d_namlen = dp->d_namlen;
#endif
		return &dir;
	}
}
