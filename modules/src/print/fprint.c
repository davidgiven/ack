/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include <varargs.h>
#include <system.h>
#include "param.h"

/*FORMAT1v $
	%s = char *
	%l = long
	%c = int
	%[uxbo] = unsigned int
	%d = int
$ */
/*VARARGS*/
fprint(va_alist)
	va_dcl
{
	File *fp;
	char *fmt;
	va_list args;

	char buf[SSIZE];

	va_start(args);
	fp = va_arg(args, File *);
	fmt = va_arg(args, char *);
	sys_write(fp, buf, _format(buf, fmt, args));
	va_end(args);
}
