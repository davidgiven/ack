/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include <system.h>

extern File *_get_entry();

int
sys_open(path, flag, filep)
	char *path;
	int flag;
	File **filep;
{
	register int fd;
	register File *fp;
	long lseek();

	if ((fp = _get_entry()) == (File *)0)
		return 0;
	switch (flag) {
	case OP_READ:
		if ((fd = open(path, 0)) < 0)
			return 0;
		break;
	case OP_APPEND:
		if ((fd = open(path, 1)) < 0) {
			if (access(path, 0) == 0)
				return 0;
		}
		else {
			if (lseek(fd, 0L, 2) < 0L) {
				close(fd);
				return 0;
			}
			break;
		}
		/* Fall through */
	case OP_WRITE:
		if ((fd = creat(path, 0666)) < 0)
			return 0;
		break;
	default:
		return 0;
	}
	fp->o_flags = flag;
	fp->o_fd = fd;
	*filep = fp;
	return 1;
}
