/*
 * vfprintf - formatted output without ellipsis
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

int vfprintf(FILE* stream, const char* format, va_list arg)
{
	return _doprnt(format, arg, stream);
}

#endif
