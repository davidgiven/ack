#include "bc_string.h"

/* $Id$ */

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

String *_nstr(f)
	double f;
{
	char buffer[80];

	_str(f, buffer);
	return (String *) _newstr(buffer);
}
