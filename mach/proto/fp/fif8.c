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

struct fif8_returns {
	_double	ipart;
	_double fpart;
};

fif8(p,x,y)
_double	x,y;
struct fif8_returns *p;
{

	EXTEND	e1,e2;

	extend(&y,&e1,sizeof(_double));
	extend(&x,&e2,sizeof(_double));
		/* do a multiply */
	mul_ext(&e1,&e2);
	e2 = e1;
	compact(&e2, &y, sizeof(_double));
	if (e1.exp < 0) {
		p->ipart.__double[0] = 0;
		p->ipart.__double[1] = 0;
		p->fpart = y;
		return;
	}
	if (e1.exp > 62 - DBL_M1LEFT) {
		p->ipart = y;
		p->fpart.__double[0] = 0;
		p->fpart.__double[1] = 0;
		return;
	}
	b64_sft(&e1.m1, 63 - e1.exp);
	b64_sft(&e1.m1, e1.exp - 63);	/* "loose" low order bits */
	compact(&e1, &(p->ipart), sizeof(DOUBLE));
	p->fpart = sbf8(p->ipart, y);
}
