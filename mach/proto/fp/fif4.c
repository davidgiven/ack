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

struct fif4_returns {
	_float ipart;
	_float fpart;
};

fif4(p,x,y)
_float	x,y;
struct fif4_returns *p;
{

	EXTEND	e1,e2;

	extend((_double *)&y,&e1,sizeof(_float));
	extend((_double *)&x,&e2,sizeof(_float));
		/* do a multiply */
	mul_ext(&e1,&e2);
	e2 = e1;
	compact(&e2, (_double *)&y, sizeof(_float));
	if (e1.exp < 0) {
		p->ipart = 0;
		p->fpart = y;
		return;
	}
	if (e1.exp > 30 - SGL_M1LEFT) {
		p->ipart = y;
		p->fpart = 0;
		return;
	}
	b64_sft(&e1.m1, 63 - e1.exp);
	b64_sft(&e1.m1, e1.exp - 63);	/* "loose" low order bits */
	compact(&e1,(_double *) &(p->ipart), sizeof(SINGLE));
	p->fpart = sbf4(p->ipart, y);
}
