/* 
 * putw - write an word on a stream
 */
/* $Id$ */

#include	<stdio.h>

int
putw(int w, register FILE *stream)
{
	register int cnt = sizeof(int);
	register char *p = (char *) &w;

	while (cnt--) {
		putc(*p++, stream);
	}
	if (ferror(stream)) return EOF;
	return w;
}
