/* $Header$ */
#include <stdio.h>
#include <varargs.h>

int sscanf(va_alist)
va_dcl
{
	char *string;		/* source of data */
	char *format;		/* control string */
	FILE _tempfile;
	va_list ap;
	int retval;

	va_start(ap);
	string = va_arg(ap, char *);
	format = va_arg(ap, char *);
	_tempfile._fd     = -1;
	_tempfile._flags  = IO_READMODE + IO_UNBUFF;
	_tempfile._buf    = (unsigned char *) string;
	_tempfile._ptr    = (unsigned char *) string;
	_tempfile._count  = 32767;

	retval = _doscanf (&_tempfile, format, ap);
	va_end(ap);
	return retval;
}
