/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <unistd.h>
#include "system.h"

void sys_close(File* fp)
{
	if (fp)
	{
		fclose(fp->fd);
		fp->fd = NULL;
	}
}
