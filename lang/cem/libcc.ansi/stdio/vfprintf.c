/*
 * vfprintf - formatted output without ellipsis
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

int vfprintf(FILE* stream, const char* format, va_list arg)
{
	return _doprnt(format, arg, stream);
}
