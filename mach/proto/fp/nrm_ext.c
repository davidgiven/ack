/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/********************************************************/
/*
	NORMALIZE an EXTENDED FORMAT NUMBER
*/
/********************************************************/

#include "FP_shift.h"
#include "FP_types.h"

nrm_ext(e1)
EXTEND	*e1;
{
	register unsigned long	*mant_1;
	register unsigned long	*mant_2;

	/* local CAST conversion	*/
	mant_1 = (unsigned long *) &e1->m1;
	mant_2 = (unsigned long *) &e1->m2;

		/* we assume that the mantissa != 0	*/
		/* if it is then just return		*/
		/* to let it be a problem elsewhere	*/
		/* THAT IS, The exponent is not set to	*/
		/* zero. If we don't test here an	*/
		/* infinite loop is generated when	*/
		/* mantissa is zero			*/

	if ((*mant_1 | *mant_2) == 0L)
		return;

		/* if top word is zero mov low word	*/
		/* to top word, adjust exponent value	*/
	if (*mant_1 == 0L)	{
		*mant_1++ = e1->m2;
		*mant_1-- = 0L;
		e1->exp -= 32;
	}
	while ((*mant_1 & NORMBIT) == 0) {
		e1->exp--;
		*mant_1 <<= 1;
		if ((*mant_2 & CARRYBIT) == 0)
				;	/* empty statement */
		else	{
			*mant_1 += 1;
		}
		*mant_2 <<= 1;
	}
}
