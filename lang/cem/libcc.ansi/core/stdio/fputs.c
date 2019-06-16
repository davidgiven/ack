/*
 * fputs - print a string
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int fputs(register const char* s, register FILE* stream)
{
	register int i = 0;

	while (*s)
		if (putc(*s++, stream) == EOF)
			return EOF;
		else
			i++;

	return i;
}

#endif
