#include "f2c.h"

double r_int(x)
real *x;
{
double floor();

return( (*x>0) ? floor(*x) : -floor(- *x) );
}
