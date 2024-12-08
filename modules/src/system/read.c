/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <unistd.h>
#include "system.h"

int sys_read(File* fp, char* bufptr, int bufsiz, int* pnbytes)
{
	if (!fp)
		return 0;
	
	FILE* fd = getfd(fp);
	*pnbytes = 0;
	int len = fread(bufptr, 1, bufsiz, fd);
	if (len < 0)
		return 0;
	*pnbytes = len;
	return 1;
}
