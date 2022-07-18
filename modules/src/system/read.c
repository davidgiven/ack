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
	
	*pnbytes = 0;
	while (bufsiz != 0)
	{
		int len = read(fp->o_fd, bufptr, bufsiz);
		if (len < 0)
			return 0;
		if (len == 0)
			return *pnbytes != 0;

		*pnbytes += len;
		bufptr += len;
		bufsiz -= len;
	}

	return 1;
}
