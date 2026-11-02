/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Id$ */

#include <stdint.h>
#include "flt_misc.h"

/* Shifts the mantissa m right by n bits; returns whether any of the bits
   shifted out was set. */
static int sft_lost(struct flt_mantissa *m, int n)
{
	uint32_t mask;
	int lost;

	if (n > 63)
		lost = (m->flt_h_32 | m->flt_l_32) != 0;
	else if (n >= 32) {
		mask = n == 32 ? 0 : (1UL << (n - 32)) - 1;
		lost = m->flt_l_32 != 0 || (m->flt_h_32 & mask) != 0;
	}
	else
		lost = (m->flt_l_32 & ((1UL << n) - 1)) != 0;
	flt_b64_sft(m, n);
	return lost;
}

void flt_add(flt_arith *e1, flt_arith *e2,flt_arith *res)
{
	flt_arith ce1, ce2;
	int diff;
	int lost = 0;	/* bits of the smaller operand shifted out */

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
		lost = sft_lost(&(e2->flt_mantissa), diff);
	}
	else if (diff > 0) {
		e1->flt_exp += diff;
		lost = sft_lost(&(e1->flt_mantissa), diff);
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
		if (lost) {
			/* the exact difference is a little less */
			if (res->m2-- == 0)
				res->m1--;
		}
	}
	else {
		*res = *e2;
		if (flt_b64_add(&res->flt_mantissa,&e1->flt_mantissa)) {/* addition carry */
			lost |= sft_lost(&res->flt_mantissa, 1);
			res->m1 |= 0x80000000L;	/* set max bit	*/
			res->flt_exp++;		/* increase the exponent */
		}
	}
	/* Bits that did not fit are kept as a sticky lowest bit (rounding to
	   odd), so that flt_rnd() can round the result once more correctly. */
	if (lost)
		res->m2 |= 1;
	flt_nrm(res);
	flt_chk(res);
}

void flt_sub(flt_arith *e1, flt_arith *e2, flt_arith *res)
{
	e2->flt_sign = ! e2->flt_sign;
	flt_add(e1,e2,res);
	e2->flt_sign = ! e2->flt_sign;
}
