#include "f2c.h"

double d_int(x)
doublereal *x;
{
double floor();

return( (*x>0) ? floor(*x) : -floor(- *x) );
}
