/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Id$ */

#include "flt_misc.h"
#include <em_arith.h>

arith
flt_flt2arith(e, uns)
	register flt_arith *e;
{
	/*	Convert the flt_arith "n" to an arith.
	*/
	arith	n;
	struct flt_mantissa a;

	if (uns) uns = 1;
	flt_status = 0;
	if (e->flt_sign && uns) {
		flt_status = FLT_UNFL;
		return 0;
	}
	if (e->flt_exp < 0) {
		/* absolute value of result < 1.
		   Return value only depends on sign:
		*/
		return -e->flt_sign;
	}

	if (e->flt_exp > 8*sizeof(arith)-2 + uns) {
		/* probably overflow, but there is one exception:
		*/
		if (e->flt_sign &&
		    e->flt_exp == 8*sizeof(arith)-1 &&
		    e->m2 == 0 &&
		    e->m1 == 0x80000000) {
			/* No overflow in this case */
			flt_status = 0;
		}
		else {
			flt_status = FLT_OVFL;
			e->flt_exp = 8*sizeof(arith)-2 + uns + e->flt_sign;
			if (e->flt_sign) {
				e->m1 = 0x80000000;
				e->m2 = 0;
			}
			else {
				e->m1 = 0xFFFFFFFF;
				e->m2 = 0xFFFFFFFF;
			}
		}
	}
	
	a = e->flt_mantissa;
	flt_b64_sft(&a, 63-e->flt_exp);
	n = a.flt_l_32 | ((a.flt_h_32 << 16) << 16);
	/* not << 32; this could be an undefined operation */
	return e->flt_sign ? -n : n;
}
