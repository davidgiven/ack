/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	SEPERATE INTO EXPONENT AND FRACTION
*/

#include	"FP_types.h"

struct	fef4_returns {
	int	e;
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
