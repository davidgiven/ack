/* $Id$ */
#include <stdio.h>

fread(ptr, size, count, file)
register char *ptr;
unsigned size, count;
register FILE *file;
{
	int c;
	unsigned ndone = 0, s;

	ndone = 0;
	if (size)
		while ( ndone < count ) {
			s = size;
			do {
				if ((c = getc(file)) != EOF)
					*ptr++ = c;
				else
					return(ndone);
			} while (--s);
			ndone++;
		}
	return(ndone);
}

