/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	SEPERATE DOUBLE INTO EXPONENT AND FRACTION (FEF 8)
*/

#include	"FP_types.h"

struct	fef8_returns	{
	int	e;
	_double	f;
};

fef8(r, s1)
_double	s1;
struct fef8_returns *r;
{
	EXTEND	buf;
	register struct fef8_returns *p = r;	/* make copy, r might refer
						   to itself (see table)
						*/

	extend(&s1,&buf,sizeof(_double));
	p->e = buf.exp + 1;
	buf.exp = -1;
	compact(&buf,&p->f,sizeof(_double));
}
