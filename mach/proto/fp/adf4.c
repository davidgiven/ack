/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	ADD TWO FLOATS - SINGLE (ADF 4)
*/

#include	"FP_types.h"

_float
adf4(s2,s1)
_float	s1,s2;
{
	EXTEND	e1,e2;
	int	swap = 0;

	if (s1 == (_float) 0) {
		s1 = s2;
		return s1;
	}
	if (s2 == (_float) 0) {
		return s1;
	}
	extend((_double *)&s1,&e1,sizeof(SINGLE));
	extend((_double *)&s2,&e2,sizeof(SINGLE));
	add_ext(&e1,&e2);
	compact(&e1,(_double *)&s1,sizeof(SINGLE));
	return(s1);
}
