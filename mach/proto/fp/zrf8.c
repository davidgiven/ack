/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	return a zero double
*/

#include "FP_types.h"

zrf8(z)
_double	*z;
{

	z->__double[0] = 0L;
	z->__double[1] = 0L;
}
