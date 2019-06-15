/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Id$ */

#include "flt_misc.h"

void flt_add(flt_arith *e1, flt_arith *e2,flt_arith *res)
{
	flt_arith ce1, ce2;
	int diff;

	flt_status = 0;
	if ((e2->m1 | e2->m2) == 0L) {
		*res = *e1;
		return;
	}
	if ((e1->m1 | e1->m2) == 0L) {
		*res = *e2;
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
		if (e1->m1 > e2->m1 ||
		    (e1->m1 == e2->m1 && e1->m2 > e2->m2)) {
                	/*      abs(e1) > abs(e2) */
			if (e1->m2 < e2->m2) {
                        	e1->m1 -= 1;    /* carry in */
                	}
                	e1->m1 -= e2->m1;
                	e1->m2 -= e2->m2;
                	*res = *e1;
        	}
        	else {
			if (e1->m2 > e2->m2)
                        	e2->m1 -= 1;    /* carry in */
                	e2->m1 -= e1->m1;
                	e2->m2 -= e1->m2;
			*res = *e2;
        	}
	}
	else {
		*res = *e2;
		if (flt_b64_add(&res->flt_mantissa,&e1->flt_mantissa)) {/* addition carry */
			flt_b64_sft(&res->flt_mantissa, 1);
			res->m1 |= 0x80000000L;	/* set max bit	*/
			res->flt_exp++;		/* increase the exponent */
		}
	}
	flt_nrm(res);
	flt_chk(res);
}

void flt_sub(flt_arith *e1, flt_arith *e2, flt_arith *res)
{
	e2->flt_sign = ! e2->flt_sign;
	flt_add(e1,e2,res);
	e2->flt_sign = ! e2->flt_sign;
}
