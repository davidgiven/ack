/*
 * putchar.c - print (or buffer) a character on the standard output stream
 */
/* $Header$ */

#include	<stdio.h>

#undef	putchar

int
putchar(int c)
{
	return putc(c, stdout);
}
