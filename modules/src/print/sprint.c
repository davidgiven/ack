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
char *
sprint(va_alist)
	va_dcl
{
	char *buf, *fmt;
	va_list args;

	va_start(args);
	buf = va_arg(args, char *);
	fmt = va_arg(args, char *);
	buf[_format(buf, fmt, args)] = '\0';
	va_end(args);
	return buf;
}
