#include "lib.h"

int _asc(String* str)
{
	if (str == 0 || str->strval == 0)
		error(3);
	return (*str->strval);
}
