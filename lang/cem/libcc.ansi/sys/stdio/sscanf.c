/*
 * sscanf - read formatted output from a string
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

static const char* input_buffer;

static int sscanf_getc(void)
{
	char c = *input_buffer;
	if (c == 0)
		return EOF;
	input_buffer++;
	return c;
}

static void sscanf_ungetc(int c)
{
	/* sscanf always ungets the last character read. */
	input_buffer--;
}

int sscanf(const char* s, const char* format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);

	input_buffer = s;
	retval = _doscan(format, ap, sscanf_getc, sscanf_ungetc);

	va_end(ap);

	return retval;
}

#endif
