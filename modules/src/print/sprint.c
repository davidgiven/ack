/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include <system.h>
#include "param.h"

/*VARARGS1*/
char *
sprint(buf, fmt, args)
	char *buf, *fmt;
	int args;
{
	buf[_format(buf, fmt, &args)] = '\0';
	return buf;
}
