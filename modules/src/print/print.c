/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include <varargs.h>
#include <system.h>
#include "param.h"

/*FORMAT0v $
	%s = char *
	%l = long
	%c = int
	%[uxbo] = unsigned int
	%d = int
$ */
/*VARARGS*/
print(va_alist)
	va_dcl
{
	char *fmt;
	va_list args;
	char buf[SSIZE];

	va_start(args);
	fmt = va_arg(args, char *);
	sys_write(STDOUT, buf, _format(buf, fmt, args));
	va_end(args);
}
