#include "f2c.h"

double d_nint(x)
doublereal *x;
{
double floor();

return( (*x)>=0 ?
	floor(*x + .5) : -floor(.5 - *x) );
}
