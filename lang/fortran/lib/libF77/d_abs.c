#include "f2c.h"

double d_abs(x)
doublereal *x;
{
if(*x >= 0)
	return(*x);
return(- *x);
}
