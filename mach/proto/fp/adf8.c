/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	ADD TWO FLOATS - DOUBLE (ADF 8)
*/

#include	"FP_types.h"

_double
adf8(s2,s1)
_double	s1,s2;
{
	EXTEND	e1,e2;

	if (s1.__double[0] == 0 && s1.__double[1] == 0) {
		s1 = s2;
		return s1;
	}
	if (s2.__double[0] == 0 && s2.__double[1] == 0) {
		return s1;
	}

	extend(&s1,&e1,sizeof(_double));
	extend(&s2,&e2,sizeof(_double));
	add_ext(&e1,&e2);
	compact(&e1,&s1,sizeof(_double));
	return s1;
}
