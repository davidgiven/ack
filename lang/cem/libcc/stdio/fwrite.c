/* $Id$ */
#include <stdio.h>

fwrite(ptr, size, count, file)
unsigned size, count;
register char *ptr;
register FILE *file;
{
	unsigned s;
	unsigned ndone = 0;

	if (size)
		while ( ndone < count ) {
			s = size;
			do {
				putc(*ptr++, file);
				if (ferror(file))
					return(ndone);
			} 
			while (--s);
			ndone++;
		}
	return(ndone);
}
