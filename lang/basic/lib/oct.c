#include "bc_string.h"

/* $Header$ */

String *_oct(i)
int i;
{
	char buffer[30];
	sprintf(buffer,"%o",i);
	return( (String *)_newstr(buffer));
}

String *_hex(i)
int i;
{
	char buffer[30];
	register char *p = buffer;
	sprintf(buffer,"%x",i);
	while (*p) {
		if (*p >= 'a' && *p <= 'f') *p += 'A'-'a';
		p++;
	}
	return( (String *)_newstr(buffer));
}
