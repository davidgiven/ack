/*
 * getc.c - read an unsigned character
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int(getc)(FILE* stream)
{
	return getc(stream);
}

#endif
