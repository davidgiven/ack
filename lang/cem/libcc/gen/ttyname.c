/* $Id$ */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>

#define DEV "/dev/"

char *
ttyname(filedes)
{
	static char result[MAXNAMLEN + 1 + 5];
	DIR *dirp;
	register struct direct *dp;
	struct stat fdstat;
	ino_t inode;
	dev_t device;
	char *name;

	if (! isatty(filedes) ||
	    fstat(filedes, &fdstat) < 0 ||
	    (fdstat.st_mode & S_IFMT) != S_IFCHR ||
	    (dirp = opendir(DEV)) == NULL) {
		return 0;
	}

	inode = fdstat.st_ino;
	device = fdstat.st_rdev;

	while ((dp = readdir(dirp)) != NULL) {
		if (dp->d_ino != inode) continue;
		strcpy(result, DEV);
		strcat(result, dp->d_name);
		if (stat(result, &fdstat) < 0) continue;
		if (fdstat.st_rdev == device &&
		    (fdstat.st_mode & S_IFMT) == S_IFCHR) {
			closedir(dirp);
			return result;
		}
	}
	closedir(dirp);
	return 0;
}
