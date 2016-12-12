#include "bc_string.h"

/* $Id$ */

String *_chr(int i)
{
	String	*s;
	char	buf[2];

	if( i<0 || i>127)
		error(3);
	buf[0]=i;
	buf[1]=0;
	s= _newstr(buf);
	return(s);
}
