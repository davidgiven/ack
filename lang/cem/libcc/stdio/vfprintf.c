/* $Header$ */

#include	<stdio.h>
#include	<varargs.h>

int
vfprintf(stream, format, arg)
	FILE *stream;
	char *format;
	va_list arg;
{
	return _doprnt (format, arg, stream);
}
