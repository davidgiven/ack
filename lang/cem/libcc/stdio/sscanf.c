#include <stdio.h>

int sscanf (string, format, args)
char           *string;		/* source of data */
char           *format;		/* control string */
unsigned        args;		/* our args */
{
	FILE _tempfile;

	_tempfile._fd    = -1;
	_tempfile._flags = IO_READMODE + IO_UNBUFF;
	_tempfile._buf   = (unsigned char *) string;
	_tempfile._ptr   = (unsigned char *) string;
	_tempfile._count = 32767;

	return _doscanf (&_tempfile, format, &args);
}
