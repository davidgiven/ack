/*
 * sprintf - print formatted output on an array
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

int sprintf(char* s, const char* format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);
	retval = vsnprintf(s, 32767, format, ap);
	va_end(ap);

	return retval;
}

#endif
