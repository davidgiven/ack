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
sprint(char *buf, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	buf[_format(buf, fmt, args)] = '\0';
	va_end(args);
	return buf;
}
