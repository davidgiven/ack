/* $Header$ */
#include <stdio.h>
#include <varargs.h>

char *sprintf(va_alist)
va_dcl
{
	char *buf, *format;
	FILE _tempfile;
	va_list ap;

	va_start(ap);
	buf = va_arg(ap, char *);
	format = va_arg(ap, char *);
	_tempfile._fd     = -1;
	_tempfile._flags  = IO_WRITEMODE + IO_UNBUFF;
	_tempfile._buf    = (unsigned char *) buf;
	_tempfile._ptr    = (unsigned char *) buf;
	_tempfile._count  = 32767;

	_doprnt(format, ap, &_tempfile);
	putc('\0',&_tempfile);

	va_end(ap);
	return buf;
}
