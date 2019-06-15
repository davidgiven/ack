/*
 * vfprintf - formatted output without ellipsis
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

static FILE* vfprintf_stream;

static void vfprintf_putc(int c)
{
	putc(c, vfprintf_stream);
}

int vfprintf(FILE* stream, const char* format, va_list arg)
{
	vfprintf_stream = stream;
	return _doprnt(format, arg, vfprintf_putc);
}

#endif
