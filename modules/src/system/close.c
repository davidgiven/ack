/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <system.h>

void
sys_close(fp)
	register File *fp;
{
	if (fp) {
		fp->o_flags = 0;
		close(fp->o_fd);
		fp->o_fd = -1;
	}
}
