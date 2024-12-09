/* $Id$ */

#include	<stdio.h>
#include	<varargs.h>

int
vprintf(format, arg)
	char *format;
	va_list arg;
{
	return _vfprintf(format, arg, stdout);
}
