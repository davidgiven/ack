/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
		NEGATE A FLOATING POINT (NGF 4)
*/
/********************************************************/

#include "FP_types.h"
#include "get_put.h"

#define OFF ((FL_MSW_AT_LOW_ADDRESS ? 0 : 2) + (FL_MSB_AT_LOW_ADDRESS ? 0 : 1))
_float
ngf4(f)
_float	f;
{
	unsigned char *p;

	if (f != (_float) 0) {
		p = (unsigned char *) &f + OFF;
		*p ^= 0x80;
	}
	return f;
}
