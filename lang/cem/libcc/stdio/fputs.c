/* $Id$ */
#include <stdio.h>

fputs(s,file)
register char *s;
register FILE *file;
{
	while ( *s ) 
		putc(*s++,file);
}
