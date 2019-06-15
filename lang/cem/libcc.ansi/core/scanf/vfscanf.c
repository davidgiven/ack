/*
 * vfscanf.c - read formatted input from stream
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "doscan.h"

#if ACKCONF_WANT_STDIO

static FILE* vfscanf_stream;

static int vfscanf_getc(void)
{
	return getc(vfscanf_stream);
}

static void vfscanf_ungetc(int c)
{
	ungetc(c, vfscanf_stream);
}

int vfscanf(FILE* stream, const char* format, va_list ap)
{
	vfscanf_stream = stream;
	_doscan_get = vfscanf_getc;
	_doscan_unget = vfscanf_ungetc;
	return _doscan(format, ap);
}

#endif
