/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <system.h>
#include "print.h"
#include "param.h"

/*FORMAT1 $
	%s = char *
	%l = long
	%c = int
	%[uxbo] = unsigned int
	%d = int
$ */
void
doprnt(fp, fmt, argp)
	File *fp;
	char *fmt;
	va_list argp;
{
	char buf[SSIZE];

	sys_write(fp, buf, _format(buf, fmt, argp));
}
