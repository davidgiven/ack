/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include <system.h>
#include "print.h"
#include "param.h"

/*FORMAT0v $
	%s = char *
	%l = long
	%c = int
	%[uxbo] = unsigned int
	%d = int
$ */
/*VARARGS*/
void
print
#if __STDC__
	(char *fmt, ...)
{
#else
	(va_alist)
	va_dcl
{
	char *fmt;
#endif
	va_list args;
	char buf[SSIZE];

#if __STDC__
	va_start(args, fmt);
#else
	va_start(args);
	fmt = va_arg(args, char *);
#endif
	sys_write(STDOUT, buf, _format(buf, fmt, args));
	va_end(args);
}
