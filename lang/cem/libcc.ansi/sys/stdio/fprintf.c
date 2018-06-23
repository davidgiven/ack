/*
 * fprintf - write output on a stream
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

int fprintf(FILE* stream, const char* format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);

	retval = _doprnt(format, ap, stream);

	va_end(ap);

	return retval;
}

#endif
