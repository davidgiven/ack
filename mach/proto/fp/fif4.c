/*
	MULTIPLY AND DISMEMBER PARTS
*/

#include "FP_types.h"
#include "FP_shift.h"

_float mlf4();
_float sbf4();

fif4(x,y)
_float	x,y;
{
	EXTEND e;

	y = mlf4(x,y);
	extend((_double *)&y,&e,sizeof(SINGLE));
	e.exp--;			/* additional bias correction */
	if (e.exp < 1) {
		x = 0;
		return;
	}
	if (e.exp > 31 - SGL_M1LEFT) {
		x = y;
		y = 0;
		return;
	}
	b64_sft(&e.m1, 64 - e.exp);
	b64_sft(&e.m1, e.exp - 64);	/* "loose" low order bits */
	e.exp++;
	compact(&e,(_double *) &x, sizeof(SINGLE));
	y = sbf4(x, y);
}
