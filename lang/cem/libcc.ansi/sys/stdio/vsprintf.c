/*
 * vsprintf - print formatted output without ellipsis on an array
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

int vsprintf(char* s, const char* format, va_list ap)
{
	return vsnprintf(s, 32767, format, ap);
}

#endif
