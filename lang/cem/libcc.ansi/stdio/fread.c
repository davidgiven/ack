/*
 * fread.c - read a number of members into an array
 */
/* $Header$ */

#include	<stdio.h>

size_t
fread(register void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int c;
	size_t ndone = 0, s;

	if (size)
		while ( ndone < nmemb ) {
			s = size;
			do {
				if ((c = getc(stream)) != EOF)
					*ptr++ = c;
				else
					return ndone;
			} while (--s);
			ndone++;
		}

	return ndone;
}
