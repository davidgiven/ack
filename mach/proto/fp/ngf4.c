/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
		NEGATE A FLOATING POINT (NGF 4)
*/
/********************************************************/
/*
	Assumes exponent is located in bytes 0 & 1
*/
/********************************************************/

#include "FP_types.h"

_float
ngf4(f)
_float	f;
{
	unsigned char *p;

	if (f != (_float) 0) {
		p = (unsigned char *) &f;
		*p ^= 0x80;
	}
	return f;
}
