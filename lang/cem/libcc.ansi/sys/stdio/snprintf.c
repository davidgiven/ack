/*
 * sprintf - print formatted output on an array
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

int snprintf(char* s, size_t len, const char* format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);

	output_buffer = s;
	output_buffer_len = len;
	retval = _doprnt(format, ap, snprintf_putc);
	snprintf_putc('\0');

	va_end(ap);

	return retval;
}

#endif
