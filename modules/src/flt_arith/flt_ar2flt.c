/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

#include "flt_misc.h"
#include <em_arith.h>

flt_arith2flt(n, e, uns)
	register arith n;
	register flt_arith *e;
{
	/*	Convert the arith "n" to a flt_arith "e".
	*/
	register int i;

	if (!uns && n < 0) {
		e->flt_sign = 1;
		n = -n;
	}
	else	e->flt_sign = 0;
	switch(sizeof(arith)) {
	case 4:
		e->m1 = n; e->m2 = 0; break;
	default:
		e->m1 = (n >> (sizeof(arith)*8 - 32)) & 0xFFFFFFFF;
		e->m2 = n << 32;
		break;
	}
	if (n == 0) {
		e->flt_exp = 0;
		return;
	}
	e->flt_exp = 63;
		
	flt_status = 0;
	flt_nrm(e);
}
