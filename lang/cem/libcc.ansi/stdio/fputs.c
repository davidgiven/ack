/*
 * fputs - print a string
 */
/* $Header$ */

#include	<stdio.h>

int
fputs(register const char *s, register FILE *stream)
{
	register int retval = 0;

	while (*s) 
		if (putc(*s++, stream) == EOF) return -1;
		else retval++;

	return retval;
}
