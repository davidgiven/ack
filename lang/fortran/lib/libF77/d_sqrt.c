#include "f2c.h"

double d_sqrt(x)
doublereal *x;
{
double sqrt();
return( sqrt(*x) );
}
