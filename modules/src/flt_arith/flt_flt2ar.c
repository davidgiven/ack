/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

#include "misc.h"
#include <em_arith.h>

arith
flt_flt2arith(e)
	register flt_arith *e;
{
	/*	Convert the flt_arith "n" to an arith.
	*/
	arith	n;
	struct flt_mantissa a;

	flt_status = 0;
	if (e->flt_exp < 0) {
		/* absolute value of result < 1.
		   Return value only depends on sign:
		*/
		return -e->flt_sign;
	}

	if (e->flt_exp > (8*sizeof(arith)-2)) {
		/* probably overflow, but there is one exception:
		*/
		if (e->flt_sign) {
			n = 0x80;
			while (n << 8) n <<= 8;
			if (e->flt_exp == 8*sizeof(arith)-1 &&
			    e->m2 == 0 &&
			    e->m1 == 0x80000000) {
				/* No overflow in this case */
			}
			else flt_status = FLT_OVFL;
			return n;
		}
		n = 0x7F;
		while ((n << 8) > 0) {
			n <<= 8;
			n |= 0xFF;
		}
		return n;
	}
	
	a = e->flt_mantissa;
	flt_b64_sft(&a, 63-e->flt_exp);
	n = a.flt_l_32 | ((a.flt_h_32 << 16) << 16);
	/* not << 32; this could be an undefined operation */
	return n;
}
