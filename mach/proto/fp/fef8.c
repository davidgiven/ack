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

fef8(s1)
_double	s1;
{
	EXTEND	buf;
	struct fef8_returns *r = (struct fef8_returns *) &s1;

	extend(&s1,&buf,sizeof(_double));
	r->e = buf.exp - 1;
	buf.exp = 1;
	compact(&buf,&r->f,sizeof(_double));
}
