/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "system.h"

#if !defined WIN32
#define O_BINARY 0
#endif

extern File *_get_entry();

int
sys_open(path, flag, filep)
	char *path;
	int flag;
	File **filep;
{
	register int fd;
	register File *fp;

	if ((fp = _get_entry()) == (File *)0)
		return 0;
	switch (flag) {
	case OP_READ:
		if ((fd = open(path, O_RDONLY|O_BINARY)) < 0)
			return 0;
		break;
	case OP_APPEND:
		if ((fd = open(path, O_WRONLY|O_BINARY)) < 0) {
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
		if ((fd = open(path, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, 0666)) < 0)
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
