/* $Id$ */
#include <stdio.h>

fputc(c, iop)
	FILE *iop;
{
	return putc(c, iop);
}
