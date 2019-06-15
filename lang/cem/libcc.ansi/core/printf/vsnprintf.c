/*
 * vsprintf - print formatted output without ellipsis on an array
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "doprnt.h"

#if ACKCONF_WANT_STDIO

static char* output_buffer;
static size_t output_buffer_len;

static int snprintf_putc(int c)
{
	if (output_buffer_len)
	{
		*output_buffer++ = c;
		output_buffer_len--;
	}
	return 0;
}

int vsnprintf(char* s, size_t len, const char* format, va_list ap)
{
	int retval;

	output_buffer = s;
	output_buffer_len = len;
	_doprnt_put = snprintf_putc;
	retval = _doprnt(format, ap);
	snprintf_putc('\0');

	return retval;
}

#endif
