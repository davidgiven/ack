#include "string.h"

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
	sprintf(buffer,"%x",i);
	return( (String *)_newstr(buffer));
}
