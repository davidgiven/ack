#include "f2c.h"

double d_asin(x)
doublereal *x;
{
double asin();
return( asin(*x) );
}
