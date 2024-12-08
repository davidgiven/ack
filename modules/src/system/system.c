/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* RCS: $Id$ */

#include "system.h"

File _sys_ftab[SYS_NOPEN];
File sys_stdin;
File sys_stdout;
File sys_stderr;

FILE* getfd(File* fp)
{
	if (fp == &sys_stdin)
		return stdin;
	if (fp == &sys_stdout)
		return stdout;
	if (fp == &sys_stderr)
		return stderr;
	return fp->fd;
}

File * _get_entry(void)
{
	File *fp;

	for (fp = &_sys_ftab[0]; fp < &_sys_ftab[SYS_NOPEN]; fp++)
	{
		if (!fp->fd)
			return fp;
	}
	return NULL;
}
