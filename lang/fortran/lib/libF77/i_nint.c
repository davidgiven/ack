#include "f2c.h"

integer i_nint(x)
real *x;
{
double floor();

return( (*x)>=0 ?
	floor(*x + .5) : -floor(.5 - *x) );
}
