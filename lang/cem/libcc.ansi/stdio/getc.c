/*
 * getc.c - read an unsigned character
 */
/* $Header$ */

#include	<stdio.h>

#undef	getc

int
getc(FILE *stream)
{
	return fgetc(stream);
}
