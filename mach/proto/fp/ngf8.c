/*
		NEGATE A FLOATING POINT
*/
/********************************************************/
/*
	Assumes exponent is located in bytes 0 & 1
*/
/********************************************************/

#include "FP_types.h"

ngf8(f)
_double	f;
{
	unsigned char	*p;

	p = (unsigned char *) &f;
	*p ^= 0x80;
}

