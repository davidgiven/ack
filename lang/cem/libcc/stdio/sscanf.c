/* $Id$ */
#include <stdio.h>
#include <varargs.h>

int sscanf(va_alist)
	va_dcl
{
	va_list ap;
	int retval;

	va_start(ap);
	{
		char *string = va_arg(ap, char *);
		char *format = va_arg(ap, char *);
		FILE _tempfile;

		_tempfile._fd     = -1;
		_tempfile._flags  = IO_READMODE + IO_UNBUFF;
		_tempfile._buf    = (unsigned char *) string;
		_tempfile._ptr    = (unsigned char *) string;
		_tempfile._count  = strlen(string);

		retval = _doscanf (&_tempfile, format, ap);
	}
	va_end(ap);

	return retval;
}
