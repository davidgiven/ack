/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <fcntl.h>
#include "system.h"

int sys_create(File** filep, char* path, int mode)
{
	int fd;
	File* fp;

	if ((fp = _get_entry()) == (File*)0)
		return 0;
	fp->fd = fopen(path, "wb");
	if (!fp->fd)
		return 0;
	*filep = fp;
	return 1;
}
