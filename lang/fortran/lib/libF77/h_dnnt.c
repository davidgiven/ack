#include "f2c.h"

extern integer s_cmp();

shortint h_dnnt(x)
doublereal *x;
{
double floor();

return( (*x)>=0 ?
	floor(*x + .5) : -floor(.5 - *x) );
}
