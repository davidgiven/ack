/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <sys/types.h>
#include <sys/stat.h>
#include "system.h"

off_t sys_filesize(char* path)
{
	struct stat st_buf;

	if (stat(path, &st_buf) != 0)
		return -1L;
	return st_buf.st_size;
}
