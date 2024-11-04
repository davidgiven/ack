/* $Id$ */
#include <stdio.h>

puts(s)
char *s;
{
	FILE *file = stdout;
	while ( *s ) 
		putc(*s++,file);
	putc('\n', file);
}
