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
fprint
#if __STDC__
	(File *fp, char *fmt, ...)
{
#else
	(va_alist)
	va_dcl
{
	File *fp;
	char *fmt;
#endif
	va_list args;
	char buf[SSIZE];

#if __STDC__
	va_start(args, fmt);
#else
	va_start(args);
	fp = va_arg(args, File *);
	fmt = va_arg(args, char *);
#endif
	sys_write(fp, buf, _format(buf, fmt, args));
	va_end(args);
}
