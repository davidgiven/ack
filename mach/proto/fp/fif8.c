/*
	MULTIPLY AND DISMEMBER PARTS
*/

#include "FP_types.h"
#include "FP_shift.h"

_double mlf8();
_double sbf8();

fif8(x,y)
_double	x,y;
{
	EXTEND e;

	y = mlf8(x,y);
	extend((_double *)&y,&e,sizeof(DOUBLE));
	e.exp--;			/* additional bias correction */
	if (e.exp < 1) {
		x.__double[0] = 0;
		x.__double[1] = 0;
		return;
	}
	if (e.exp > 63 - DBL_M1LEFT) {
		x.__double[0] = y.__double[0];
		x.__double[1] = y.__double[1];
		y.__double[0] = 0;
		y.__double[1] = 0;
		return;
	}
	b64_sft(&e.m1, 64 - e.exp);
	b64_sft(&e.m1, e.exp - 64);	/* "loose" low order bits */
	e.exp++;
	compact(&e, &x, sizeof(DOUBLE));
	y = sbf8(x, y);
}
