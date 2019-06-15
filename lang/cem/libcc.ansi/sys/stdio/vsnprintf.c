/*
 * vsprintf - print formatted output without ellipsis on an array
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

static char* output_buffer;
static size_t output_buffer_len;

static void snprintf_putc(int c)
{
	if (output_buffer_len)
	{
		*output_buffer++ = c;
		output_buffer_len--;
	}
}

int vsnprintf(char* s, size_t len, const char* format, va_list ap)
{
	int retval;

	output_buffer = s;
	output_buffer_len = len;
	retval = _doprnt(format, ap, snprintf_putc);
	snprintf_putc('\0');

	return retval;
}

#endif
