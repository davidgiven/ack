/*
 * scanf.c - read formatted input from the standard input stream
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>

#if ACKCONF_WANT_STDIO

int scanf(const char* format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);
	retval = vfscanf(stdin, format, ap);
	va_end(ap);

	return retval;
}

#endif
