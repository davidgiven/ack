/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
 * Multiply Single Precesion Float (MLF 4)
 */

#include	"FP_types.h"

_float
mlf4(s2,s1)
_float	s1,s2;
{
	EXTEND	e1,e2;

	extend((_double *)&s1,&e1,sizeof(_float));
	extend((_double *)&s2,&e2,sizeof(_float));
		/* do a multiply */
	mul_ext(&e1,&e2);
	compact(&e1,(_double *)&s1,sizeof(_float));
	return(s1);
}
