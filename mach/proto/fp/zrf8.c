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
