/* $Header$ */
/* SYSTEM DEPENDENT ROUTINES */

#include "system.h"
#include "inputtype.h"
#include <sys/stat.h>

extern long lseek();

int
xopen(name, flag, mode)
	char *name;
{
	if (name[0] == '-' && name[1] == '\0')
		return (flag == OP_RDONLY) ? 0 : 1;

	switch (flag) {

	case OP_RDONLY:
		return open(name, 0);
	case OP_WRONLY:
		return open(name, 1);
	case OP_CREAT:
		return creat(name, mode);
	case OP_APPEND:
		{
			register fd;

			if ((fd = open(name, 1)) < 0)
				return -1;
			lseek(fd, 0L, 2);
			return fd;
		}
	}
	/*NOTREACHED*/
}

int
xclose(fildes)
{
	if (fildes != 0 && fildes != 1)
		return close(fildes);
	return -1;
}

#ifdef	READ_IN_ONE
long
xfsize(fildes)
{
	struct stat stbuf;

	if (fstat(fildes, &stbuf) != 0)
		return -1;
	return stbuf.st_size;
}
#endif	READ_IN_ONE

exit(n)
{
	_exit(n);
}

xstop(how, stat)
{
	switch (how) {
	case S_ABORT:
		abort();
	case S_EXIT:
		exit(stat);
	}
	/*NOTREACHED*/
}
