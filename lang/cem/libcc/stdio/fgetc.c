/* $Id$ */
#include <stdio.h>

fgetc(f)
	register FILE *f;
{
	return getc(f);
}
