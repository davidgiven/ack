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
char *
sprint
#if __STDC__
	(char *buf, char *fmt, ...)
{
#else
	(va_alist)
	va_dcl
{
	char *buf, *fmt;
#endif
	va_list args;

#if __STDC__
	va_start(args, fmt);
#else
	va_start(args);
	buf = va_arg(args, char *);
	fmt = va_arg(args, char *);
#endif
	buf[_format(buf, fmt, args)] = '\0';
	va_end(args);
	return buf;
}
