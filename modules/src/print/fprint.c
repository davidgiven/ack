/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <system.h>
#include "print.h"
#include "param.h"

/*FORMAT1v $
	%s = char *
	%l = long
	%c = int
	%[uxbo] = unsigned int
	%d = int
$ */
/*VARARGS*/
void
fprint(File *fp, const char *fmt, ...)
{
	va_list args;
	char buf[SSIZE];

	va_start(args, fmt);
	sys_write(fp, buf, _format(buf, fmt, args));
	va_end(args);
}
