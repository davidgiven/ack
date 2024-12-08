/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "system.h"

int sys_open(char* path, int flag, File** filep)
{
	File* fp;
	FILE* fd;

	if ((fp = _get_entry()) == (File*)0)
		return 0;
	switch (flag)
	{
		case OP_READ:
			if (!(fd = fopen(path, "rb")))
				return 0;
			break;
		case OP_APPEND:
			if (!(fd = fopen(path, "a+b")))
			{
				if (access(path, 0) == 0)
					return 0;
			}
			else
			{
				if (fseek(fd, 0L, SEEK_SET) < 0L)
				{
					fclose(fd);
					return 0;
				}
				break;
			}
			/* Fall through */
		case OP_WRITE:
			if (!(fd = fopen(path, "w+b")))
				return 0;
			break;
		default:
			return 0;
	}
	fp->fd = fd;
	*filep = fp;
	return 1;
}
