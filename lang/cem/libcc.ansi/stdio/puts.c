/*
 * puts.c - print a string onto the standard output stream
 */
/* $Header$ */

#include	<stdio.h>

int
puts(register const char *s)
{
	return fputs(s, stdout);
}
