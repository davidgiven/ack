/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

#include "flt_misc.h"

flt_add(e1,e2,e3)
	register flt_arith *e1,*e2,*e3;
{
	/*	Add two extended numbers e1 and e2, and put the result
		in e3
	*/
	flt_arith ce1, ce2;
	int diff;

	flt_status = 0;
	if ((e2->m1 | e2->m2) == 0L) {
		*e3 = *e1;
		return;
	}
	if ((e1->m1 | e1->m2) == 0L) {
		*e3 = *e2;
		return;
	}
	ce2 = *e2;
	ce1 = *e1;
	e1 = &ce1;
	e2 = &ce2;

	/* adjust mantissas to equal power */
	diff = e2->flt_exp - e1->flt_exp;
	if (diff < 0) {
		diff = -diff;
		e2->flt_exp += diff;
		flt_b64_sft(&(e2->flt_mantissa), diff);
	}
	else if (diff > 0) {
		e1->flt_exp += diff;
		flt_b64_sft(&(e1->flt_mantissa), diff);
	}
	if (e1->flt_sign != e2->flt_sign) {
		/* e2 + e1 = e2 - (-e1) */
		int tmp = ucmp(e1->m1, e2->m1);
		int tmp2 = ucmp(e1->m2, e2->m2);
		if (tmp > 0 || (tmp == 0 && tmp2 > 0)) {
                	/*      abs(e1) > abs(e2) */
                	if (tmp2 < 0) {
                        	e1->m1 -= 1;    /* carry in */
                	}
                	e1->m1 -= e2->m1;
                	e1->m2 -= e2->m2;
                	*e3 = *e1;
        	}
        	else {
                	if (tmp2 > 0)
                        	e2->m1 -= 1;    /* carry in */
                	e2->m1 -= e1->m1;
                	e2->m2 -= e1->m2;
			*e3 = *e2;
        	}
	}
	else {
		*e3 = *e2;
		if (flt_b64_add(&e3->flt_mantissa,&e1->flt_mantissa)) {/* addition carry */
			flt_b64_sft(&e3->flt_mantissa, 1);
			e3->m1 |= 0x80000000L;	/* set max bit	*/
			e3->flt_exp++;		/* increase the exponent */
		}
	}
	flt_nrm(e3);
	flt_chk(e3);
}

flt_sub(e1,e2,e3)
	flt_arith *e1,*e2,*e3;
{
	e2->flt_sign = ! e2->flt_sign;
	flt_add(e1,e2,e3);
	e2->flt_sign = ! e2->flt_sign;
}
