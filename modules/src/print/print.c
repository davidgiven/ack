/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include <system.h>
#include "param.h"

/*VARARGS1*/
print(fmt, args)
	char *fmt;
	int args;
{
	char buf[SSIZE];

	sys_write(STDOUT, buf, _format(buf, fmt, &args));
}
