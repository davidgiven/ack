/*
 * fputc.c - print an unsigned character
 */
/* $Id$ */

#include <stdio.h>

int fputc(int c, FILE* stream)
{
	return putc(c, stream);
}
