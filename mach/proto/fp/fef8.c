/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	SEPERATE DOUBLE INTO EXPONENT AND FRACTION
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

#ifdef	DEBUG
	printf("FEF8(): ");
#endif	DEBUG
	extend(&s1,&buf,sizeof(_double));
	r->e = buf.exp - 1;
	buf.exp = 1;
	compact(&buf,&r->f,sizeof(_double));
#ifdef	DEBUG
	printf("exponent = %3d fraction = 0x%08X%08X: ",
		r->f.__double[0],r->f.__double[1]);
	printf("FEF8()\n");
#endif	DEBUG
}
