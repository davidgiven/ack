/*
  (c) copyright 1989 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

# include "flt_misc.h"

int
flt_b64_add(e1,e2)
	register struct flt_mantissa *e1,*e2;
{
	int	overflow;
	int	carry;

	/* add higher pair of 32 bits */
	overflow = ucmp((long)0xFFFFFFFF - e1->flt_h_32, e2->flt_h_32) < 0;
	e1->flt_h_32 += e2->flt_h_32;

	/* add lower pair of 32 bits */
	carry = ucmp((long)0xFFFFFFFF - e1->flt_l_32, e2->flt_l_32) < 0;
	e1->flt_l_32 += e2->flt_l_32;

	if ((carry) && ((++e1->flt_h_32 &~0xFFFFFFFF) || e1->flt_h_32 == 0)) {
		e1->flt_h_32 = 0;
		return(1);		/* had a 64 bit overflow */
	}
	return(overflow);	/* return status from higher add */
}
