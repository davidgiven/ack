/*
	SEPERATE INTO EXPONENT AND FRACTION
*/

#include	"FP_types.h"

struct	fef4_returns {
	short	e;
	_float	f;
};

fef4(s1)
_float	s1;
{
	struct	fef4_returns	*r = (struct fef4_returns *) &s1;
	EXTEND	buf;

	extend((_double *) &s1,&buf,sizeof(_float));
	r->e = buf.exp-1;
	buf.exp = 1;
	compact(&buf,(_double *) &r->f,sizeof(_float));
}
