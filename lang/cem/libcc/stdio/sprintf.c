#include <stdio.h>

char *sprintf(buf,format,args)
char *buf, *format;
int args;
{
	FILE _tempfile;

	_tempfile._fd    = -1;
	_tempfile._flags = IO_WRITEMODE + IO_UNBUFF;
	_tempfile._buf   = (unsigned char *) buf;
	_tempfile._ptr   = (unsigned char *) buf;
	_tempfile._count = 32767;

	_doprintf(format, &args, &_tempfile);
	putc('\0',&_tempfile);

	return buf;
}
