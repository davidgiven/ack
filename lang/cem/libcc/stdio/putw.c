/* $Id$ */
#include <stdio.h>

int
putw(w, iop)
	FILE *iop;
{
	int cnt = sizeof(int);
	char *p = (char *) &w;

	while (cnt--) {
		putc(*p++, iop);
	}
	if (ferror(iop)) return EOF;
	return w;
}
