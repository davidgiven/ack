/*
 (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	SUBTRACT TWO FLOATS - SINGLE Precision (SBF 4)
*/

#include	"FP_types.h"

SINGLE
sbf4(s2,s1)
SINGLE	s1,s2;
{
	SINGLE *result = &s1;	/* s1 may not be in a register! */

	if (s2 == (SINGLE) 0) {
		return s1;
	}
	s2 = ngf4(s2);
	*result = adf4(s2,s1);
	return(s1);	/* add and return result */
}

