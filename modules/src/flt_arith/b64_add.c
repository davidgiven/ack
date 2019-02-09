/*
  (c) copyright 1989 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Id$ */

#include "flt_misc.h"

int flt_b64_add(register struct flt_mantissa *e1,register struct flt_mantissa *e2)
{
	int	overflow;
	int	carry;

	/* add higher pair of 32 bits */
	overflow = (0xFFFFFFFFUL - e1->flt_h_32 < e2->flt_h_32);
	e1->flt_h_32 += e2->flt_h_32;

	/* add lower pair of 32 bits */
	carry = (0xFFFFFFFFUL - e1->flt_l_32 < e2->flt_l_32);
	e1->flt_l_32 += e2->flt_l_32;

	if ((carry) && (++e1->flt_h_32 == 0))
		return(1);		/* had a 64 bit overflow */

	return(overflow);	/* return status from higher add */
}
