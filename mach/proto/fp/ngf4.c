/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

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

	if (f != (_float) 0) {
		p = (char unsigned *) &f;
		*p ^= 0x80;
	}
}

