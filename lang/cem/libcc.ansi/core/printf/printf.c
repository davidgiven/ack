/*
 * printf - write on the standard output stream
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>

#if ACKCONF_WANT_STDIO

int printf(const char* format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);
	retval = vfprintf(stdout, format, ap);
	va_end(ap);

	return retval;
}

#endif
