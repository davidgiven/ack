/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
		CONVERT DOUBLE TO FLOAT (CFF 8 4)

	This routine works quite simply. A floating point
	of size 08 is converted to extended format.
	This extended variable is converted back to
	a floating point of size 04.

*/

#include	"FP_types.h"

_float
cff4(src)
_double	src;	/* the source itself -	THIS TIME it's DOUBLE */
{
	EXTEND	buf;

	extend(&src,&buf,8);	/* no matter what */
	compact(&buf,(_double *) &(src.__double[1]),4);
	return *(_float *)&(src.__double[1]);
}
