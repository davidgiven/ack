/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
#define	PRT_EXT
	ADD TWO EXTENDED FORMAT NUMBERS
*/

#include "FP_types.h"

add_ext(e1,e2)
register EXTEND	*e1,*e2;
{
#ifdef	PRT_EXT
	prt_ext("before ADD #1:",e1);
	prt_ext("before ADD #2:",e2);
#endif	PRT_EXT
	if (b64_add(&e1->m1,&e2->m1)) {	/* addition carry */
		b64_sft(&e1->m1,1);	/* shift mantissa one bit RIGHT */
		e1->m1 |= 0x80000000L;	/* set max bit	*/
		e1->exp++;		/* increase the exponent */
	}
#ifdef	PRT_EXT
	prt_ext("AFTER  ADD   :",e1);
#endif
	nrm_ext(e1);
#ifdef	PRT_EXT
	prt_ext("AFTER  NRM   :",e1);
#endif
}
