/* $Id$ */
#include <stdio.h>

int getw(iop)
	FILE *iop;
{
	int cnt = sizeof(int);
	int w;
	char *p = (char *) &w;

	while (cnt--) {
		*p++ = getc(iop);
	}
	if (feof(iop) || ferror(iop)) return EOF;
	return w;
}
