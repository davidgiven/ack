/* $Id$ */
#include <stdio.h>

fputs(s,file)
char *s;
FILE *file;
{
	while ( *s ) 
		putc(*s++,file);
}
