/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <system.h>

extern File *_get_entry();

int
sys_create(filep, path, mode)
	File **filep;
	char *path;
	int mode;
{
	register fd;
	register File *fp;

	if ((fp = _get_entry()) == (File *)0)
		return 0;
	if ((fd = creat(path, mode)) < 0)
		return 0;
	fp->o_fd = fd;
	fp->o_flags = OP_WRITE;
	*filep = fp;
	return 1;
}
