/* $Header$ */

#include	<stdio.h>
#include	<varargs.h>

char *
vsprintf(s, format, arg)
	char *s;
	char *format;
	va_list arg;
{
	FILE tmp_stream;

	tmp_stream._fd     = -1;
	tmp_stream._flags  = IO_WRITEMODE + IO_UNBUFF;
	tmp_stream._buf    = (unsigned char *) s;
	tmp_stream._ptr    = (unsigned char *) s;
	tmp_stream._count  = 32767;

	_doprnt(format, arg, &tmp_stream);
	putc('\0',&tmp_stream);

	return s;
}
