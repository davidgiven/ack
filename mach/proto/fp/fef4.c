/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	SEPERATE INTO EXPONENT AND FRACTION (FEF 4)
*/

#include	"FP_types.h"

struct	fef4_returns {
	int	e;
	_float	f;
};

fef4(r,s1)
_float	s1;
struct fef4_returns	*r;
{
	EXTEND	buf;
	register struct fef4_returns	*p = r;	/* make copy; r might refer
						   to itself (see table)
						*/

	extend((_double *) &s1,&buf,sizeof(_float));
	if (buf.exp == 0 && buf.m1 == 0 && buf.m2 == 0) {
		p->e = 0;
	}
	else {
		p->e = buf.exp+1;
		buf.exp = -1;
	}
	compact(&buf,(_double *) &p->f,sizeof(_float));
}
