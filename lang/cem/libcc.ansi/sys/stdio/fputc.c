/*
 * fputc.c - print an unsigned character
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int fputc(int c, FILE* stream)
{
	return putc(c, stream);
}

#endif
