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
	
	while (nbytes != 0)
	{
		int len = write(fp->o_fd, bufptr, nbytes);
		if (len < 0)
			return 0;

		bufptr += len;
		nbytes -= len;
	}

	return 1;
}

