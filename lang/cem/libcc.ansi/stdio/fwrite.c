/*
 * fwrite.c - write a number of array elements on a file
 */
/* $Header$ */

#include	<stdio.h>

size_t
fwrite(register const void *ptr, size_t size, size_t nmemb,
	    register FILE *stream)
{
	unsigned s;
	unsigned ndone = 0;

	if (size)
		while ( ndone < nmemb ) {
			s = size;
			do {
				if (putc((int)*(unsigned char *)ptr, stream)
					== EOF)
					return(ndone);
				ptr++;
			} 
			while (--s);
			ndone++;
		}
	return(ndone);
}
