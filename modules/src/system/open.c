/* $Header$ */

#include <system.h>

extern File *_get_entry();

int
sys_open(path, flag, filep)
	char *path;
	int flag;
	File **filep;
{
	register fd;
	register File *fp;
	int open_mode;
	long lseek();

	switch (flag) {
	case OP_READ:
		open_mode = 0;
		break;
	case OP_WRITE:
	case OP_APPEND:
		open_mode = 1;
		break;
	default:
		return 0;
	}
	if ((fp = _get_entry()) == (File *)0)
		return 0;
	if (flag == OP_WRITE) {
		if ((fd = creat(path, 0644)) < 0)
			return 0;
	}
	else /* OP_READ or OP_APPEND */
	if ((fd = open(path, open_mode)) < 0) {
		if (flag == OP_READ || access(path, 0) == 0)
			return 0;
		/* now: flag == OP_APPEND */
		if ((fd = creat(path, 0644)) < 0)
			return 0;
	}
	else {
		if (flag == OP_APPEND && (lseek(fd, 0L, 2) < 0L)) {
			close(fd);
			return 0;
		}
	}
	fp->o_flags = flag;
	fp->o_fd = fd;
	*filep = fp;
	return 1;
}
