/* $Id$ */
#include <stdio.h>

fputc(c, iop)
	register FILE *iop;
{
	return putc(c, iop);
}
