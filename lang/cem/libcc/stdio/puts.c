/* $Id$ */
#include <stdio.h>

puts(s)
register char *s;
{
	register FILE *file = stdout;
	while ( *s ) 
		putc(*s++,file);
	putc('\n', file);
}
