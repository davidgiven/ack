/*
 * puts.c - print a string onto the standard output stream
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int puts(const char* s)
{
	FILE* file = stdout;
	int i = 0;

	while (*s)
	{
		if (putc(*s++, file) == EOF)
			return EOF;
		else
			i++;
	}
	if (putc('\n', file) == EOF)
		return EOF;
	return i + 1;
}

#endif
