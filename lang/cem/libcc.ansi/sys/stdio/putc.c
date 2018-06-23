/*
 * putc.c - print (or buffer) one character
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int(putc)(int c, FILE* stream)
{
	return putc(c, stream);
}

#endif
