/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <unistd.h>
#include "system.h"

int sys_write(File* fp, char* bufptr, int nbytes)
{
	if (!fp)
		return 0;

	FILE* fd = getfd(fp);
	int len = fwrite(bufptr, 1, nbytes, fd);
	return (len == nbytes);
}
