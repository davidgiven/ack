/*
 * fscanf.c - read formatted input from stream
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

int fscanf(FILE* stream, const char* format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);

	retval = _doscan(stream, format, ap);

	va_end(ap);

	return retval;
}

#endif
