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
doprnt(FILE* fp, const char *fmt, va_list argp)
{
	char buf[SSIZE];

	fwrite(buf, 1, _format(buf, fmt, argp), fp);
}
