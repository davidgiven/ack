/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	SUBTRACT TWO FLOATS - SINGLE Precision
*/

#include	"FP_types.h"

extern	_float	adf4();

_float
sbf4(s2,s1)
_float	s1,s2;
{
				/* changing the sign directly	*/
				/* is faster than the code:	*/
				/*		s2 = -s2	*/
	char unsigned *p;

	p = (char unsigned *) &s2;
	*p ^= 0x80;	/* change sign of s2 */
	s1 = adf4(s2,s1);
	return(s1);	/* add and return result */
}

