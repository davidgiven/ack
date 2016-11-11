/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Id$ */

#include "flt_misc.h"
#include <em_arith.h>

void
flt_arith2flt(n, e, uns)
	register arith n;
	register flt_arith *e;
{
	/*	Convert the arith "n" to a flt_arith "e".
	*/

	if (!uns && n < 0) {
		e->flt_sign = 1;
		n = -n;
	}
	else	e->flt_sign = 0;
	if (sizeof(arith) == 4) {
		e->m1 = 0; e->m2 = n;
	}
	else {
		/* assuming sizeof(arith) >= 8 */
		e->m1 = n >> 32;
		e->m2 = n;
	}
	if (n == 0) {
		e->flt_exp = 0;
		return;
	}
	e->flt_exp = 63;
		
	flt_status = 0;
	flt_nrm(e);
}
