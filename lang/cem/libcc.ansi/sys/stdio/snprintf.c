/*
 * sprintf - print formatted output on an array
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

int snprintf(char* s, size_t len, const char* format, ...)
{
	va_list ap;
	int retval;
	FILE tmp_stream;

	va_start(ap, format);

	tmp_stream._fd = -1;
	tmp_stream._flags = _IOWRITE + _IONBF + _IOWRITING;
	tmp_stream._buf = (unsigned char*)s;
	tmp_stream._ptr = (unsigned char*)s;
	tmp_stream._count = len;

	retval = _doprnt(format, ap, &tmp_stream);
	putc('\0', &tmp_stream);

	va_end(ap);

	return retval;
}

#endif
