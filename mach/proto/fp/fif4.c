/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	MULTIPLY AND DISMEMBER PARTS (FIF 4)
*/

#include "FP_types.h"
#include "FP_shift.h"

_float sbf4();

fif4(x,y)
_float	x,y;
{

	EXTEND	e1,e2;

	extend((_double *)&y,&e1,sizeof(_float));
	extend((_double *)&x,&e2,sizeof(_float));
		/* do a multiply */
	mul_ext(&e1,&e2);
	e2 = e1;
	compact(&e2, (_double *)&y, sizeof(_float));
	e1.exp--;			/* additional bias correction */
	if (e1.exp < 1) {
		x = 0;
		return;
	}
	if (e1.exp > 31 - SGL_M1LEFT) {
		x = y;
		y = 0;
		return;
	}
	b64_sft(&e1.m1, 64 - e1.exp);
	b64_sft(&e1.m1, e1.exp - 64);	/* "loose" low order bits */
	e1.exp++;
	compact(&e1,(_double *) &x, sizeof(SINGLE));
	y = sbf4(x, y);
}
