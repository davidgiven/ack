/*
 * putc.c - print (or buffer) one character
 */
/* $Header$ */

#include	<stdio.h>

#undef	putc

int
putc(int c, FILE *stream)
{
	return fputc(c, stream);
}
