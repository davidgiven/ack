/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	MULTIPLY AND DISMEMBER PARTS (FIF 8)
*/

#include "FP_types.h"
#include "FP_shift.h"

_double sbf8();

fif8(x,y)
_double	x,y;
{

	EXTEND	e1,e2;

	extend(&y,&e1,sizeof(_double));
	extend(&x,&e2,sizeof(_double));
		/* do a multiply */
	mul_ext(&e1,&e2);
	e2 = e1;
	compact(&e2, &y, sizeof(_double));
	e1.exp--;			/* additional bias correction */
	if (e1.exp < 1) {
		x.__double[0] = 0;
		x.__double[1] = 0;
		return;
	}
	if (e1.exp > 63 - DBL_M1LEFT) {
		x.__double[0] = y.__double[0];
		x.__double[1] = y.__double[1];
		y.__double[0] = 0;
		y.__double[1] = 0;
		return;
	}
	b64_sft(&e1.m1, 64 - e1.exp);
	b64_sft(&e1.m1, e1.exp - 64);	/* "loose" low order bits */
	e1.exp++;
	compact(&e1, &x, sizeof(DOUBLE));
	y = sbf8(x, y);
}
