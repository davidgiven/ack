/*
		NEGATE A FLOATING POINT
*/
/********************************************************/
/*
	Assumes exponent is located in bytes 0 & 1
*/
/********************************************************/

#include "FP_types.h"

ngf4(f)
_float	f;
{
	char unsigned	*p;

	p = (char unsigned *) &f;
	*p ^= 0x80;
}

