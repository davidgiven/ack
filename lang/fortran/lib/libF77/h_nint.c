#include "f2c.h"

extern integer s_cmp();

shortint h_nint(x)
real *x;
{
double floor();

return( (*x)>=0 ?
	floor(*x + .5) : -floor(.5 - *x) );
}
