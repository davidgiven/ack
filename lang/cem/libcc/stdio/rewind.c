/* $Id$ */
#include <stdio.h>

rewind(iop)
	FILE *iop;
{
	return fseek(iop, 0L, 0);
}
