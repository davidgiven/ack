/* $Id$ */
#include <stdio.h>

int
putw(w, iop)
	register FILE *iop;
{
	register int cnt = sizeof(int);
	register char *p = (char *) &w;

	while (cnt--) {
		putc(*p++, iop);
	}
	if (ferror(iop)) return EOF;
	return w;
}
