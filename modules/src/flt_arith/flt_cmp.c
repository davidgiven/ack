/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

#include	"misc.h"

int
flt_cmp(e1, e2)
	register flt_arith *e1, *e2;
{
	int sign;
	int tmp;

	if (e1->flt_exp == 0 && e1->m1 == 0 && e1->m2 == 0 &&
	    e2->flt_exp == 0 && e2->m1 == 0 && e2->m2 == 0) {
		return 0;
	}
	sign = e1->flt_sign ? -1 : 1;
	if (e1->flt_sign > e2->flt_sign) return -1;
	if (e1->flt_sign < e2->flt_sign) return 1;
	if (e1->flt_exp < e2->flt_exp) return -sign;
	if (e1->flt_exp > e2->flt_exp) return sign;
	if ((tmp = ucmp(e1->m1, e2->m1)) < 0) return -sign;
	if (tmp > 0) return sign;
	if ((tmp = ucmp(e1->m2, e2->m2)) < 0) return -sign;
	if (tmp > 0) return sign;
	return 0;
}
