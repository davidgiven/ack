/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <unistd.h>
#include "system.h"

int
sys_seek(File* fp, long off, int whence, long* poff)
{
	if (! fp) return 0;
	return (*poff = lseek(fp->o_fd, off, whence)) >= 0;
}
