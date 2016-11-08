/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Id$ */

#include "flt_misc.h"

void
flt_b64_sft(e,n)
	register struct flt_mantissa *e;
	register int n;
{
	if (n > 63 || n < -63) {
		e->flt_l_32 = 0;
		e->flt_h_32 = 0;
		return;
	}
	if (n >= 32) {
		e->flt_l_32 = e->flt_h_32;
		e->flt_h_32 = 0;
		n -= 32;
	}
	if (n > 0) {
		e->flt_l_32 >>= n;
		if (e->flt_h_32 != 0) {
			e->flt_l_32 |= (e->flt_h_32 << (32 - n));
			e->flt_h_32 >>= n;
		}
	}
	n = -n;
	if (n >= 32) {
		e->flt_h_32 = e->flt_l_32;
		e->flt_l_32 = 0;
		n -= 32;
	}
	if (n > 0) {
		e->flt_h_32 <<= n;
		if (e->flt_l_32 != 0) {
			e->flt_h_32 |= (e->flt_l_32 >> (32 - n));
			e->flt_l_32 <<= n;
		}
	}
}
