#include "f2c.h"

double r_asin(x)
real *x;
{
double asin();
return( asin(*x) );
}
