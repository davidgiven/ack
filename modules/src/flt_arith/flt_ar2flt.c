/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

#include "misc.h"
#include <em_arith.h>

flt_arith2flt(n, e)
	register arith n;
	register flt_arith *e;
{
	/*	Convert the arith "n" to a flt_arith "e".
	*/
	register int i;

	if (n == 0) {
		e->flt_exp = 0;
		e->flt_sign = 0;
		e->m1 = e->m2 = 0;
		return;
	}
	if (n < 0) {
		e->flt_sign = 1;
		n = -n;
	}
	else	e->flt_sign = 0;
	e->flt_exp = 63;
	e->m1 = e->m2 = 0;
	if (n < 0) {
		n = 0x40000000;
		while ((n << 1) > 0) n <<= 1;
		e->flt_exp++;
	}
	for (i = 64; i > 0 && n != 0; i--) {
		b64_rsft(&(e->flt_mantissa));
		e->m1 |= (n & 1) << 31;
		n >>= 1;
	}

	if (i > 0) {
		b64_sft(&(e->flt_mantissa), i);
	}
	flt_status = 0;
	flt_nrm(e);
}
