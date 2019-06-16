/*
 * sprintf - print formatted output on an array
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>

#if ACKCONF_WANT_STDIO

int snprintf(char* s, size_t len, const char* format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);
	retval = vsnprintf(s, len, format, ap);
	va_end(ap);

	return retval;
}

#endif
