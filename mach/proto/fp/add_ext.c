/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	ADD TWO EXTENDED FORMAT NUMBERS
*/

#include "FP_types.h"

add_ext(e1,e2)
register EXTEND	*e1,*e2;
{
	if (b64_add(&e1->m1,&e2->m1)) {	/* addition carry */
		b64_sft(&e1->m1,1);	/* shift mantissa one bit RIGHT */
		e1->m1 |= 0x80000000L;	/* set max bit	*/
		e1->exp++;		/* increase the exponent */
	}
	nrm_ext(e1);
}
