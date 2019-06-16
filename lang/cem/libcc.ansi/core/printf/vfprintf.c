/*
 * vfprintf - formatted output without ellipsis
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "doprnt.h"

#if ACKCONF_WANT_STDIO

static FILE* vfprintf_stream;

static int vfprintf_putc(int c)
{
	putc(c, vfprintf_stream);
	return ferror(vfprintf_stream);
}

int vfprintf(FILE* stream, const char* format, va_list arg)
{
	vfprintf_stream = stream;
	_doprnt_put = vfprintf_putc;
	return _doprnt(format, arg);
}

#endif
