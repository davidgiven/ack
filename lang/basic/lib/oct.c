#include <stdlib.h>
#include <stdio.h>
#include "lib.h"

String* _oct(int i)
{
	char buffer[30];
	sprintf(buffer, "%o", i);
	return ((String*)_newstr(buffer));
}

String* _hex(int i)
{
	char buffer[30];

	sprintf(buffer, "%x", i);
	return ((String*)_newstr(buffer));
}

String* _nstr(double f)
{
	char buffer[80];

	_str(f, buffer);
	return (String*)_newstr(buffer);
}
