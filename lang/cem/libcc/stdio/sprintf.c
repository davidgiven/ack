/* $Id$ */
#include <stdio.h>
#include <varargs.h>

char *sprintf(va_alist)
	va_dcl
{
	va_list ap;
	char *retval;

	va_start(ap);
	{
		char *buf = va_arg(ap, char *);
		char *format = va_arg(ap, char *);
		FILE _tempfile;

		retval = buf;
		_tempfile._fd     = -1;
		_tempfile._flags  = IO_WRITEMODE + IO_UNBUFF;
		_tempfile._buf    = (unsigned char *) buf;
		_tempfile._ptr    = (unsigned char *) buf;
		_tempfile._count  = 32767;

		_doprnt(format, ap, &_tempfile);
		putc('\0',&_tempfile);
	}
	va_end(ap);

	return retval;
}
