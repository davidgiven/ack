/* $Id$ */
#include <stdio.h>

int getw(iop)
	register FILE *iop;
{
	register int cnt = sizeof(int);
	int w;
	register char *p = (char *) &w;

	while (cnt--) {
		*p++ = getc(iop);
	}
	if (feof(iop) || ferror(iop)) return EOF;
	return w;
}
