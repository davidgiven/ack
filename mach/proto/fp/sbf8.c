/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	SUBTRACT TWO FLOATS - DOUBLE Precision
*/

#include	"FP_types.h"

extern	_double	adf8();

_double
sbf8(s2,s1)
_double	s1,s2;
{
				/* changing the sign directly	*/
				/* is faster than the code line	*/
				/*	s2 = -s2;		*/
	char unsigned *p;		/* sufficient to access sign bit */

#ifdef	PRT_EXT
	fprintf(stderr,"SBF8 ():\n");
#endif
	if (s2.__double[0] == 0 && s2.__double[1] == 0) {
		return s1;
	}
	p = (char unsigned *) &s2;
	*p ^= 0x80;	/* change sign of s2 */
	s1 = adf8(s2,s1);	/* add and return result */
	return(s1);
}

