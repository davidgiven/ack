#include "f2c.h"

double r_sqrt(x)
real *x;
{
double sqrt();
return( sqrt(*x) );
}
