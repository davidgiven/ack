/*
 * fputs - print a string
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int fputs(const char* s, FILE* stream)
{
	int i = 0;

	while (*s)
		if (putc(*s++, stream) == EOF)
			return EOF;
		else
			i++;

	return i;
}

#endif
