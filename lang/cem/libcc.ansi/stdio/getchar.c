/*
 * getchar.c - read a character from the standard input stream
 */
/* $Header$ */

#include	<stdio.h>

#undef	getchar

int
getchar(void)
{
	return getc(stdin);
}
