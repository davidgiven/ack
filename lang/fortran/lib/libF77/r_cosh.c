#include "f2c.h"

double r_cosh(x)
real *x;
{
double cosh();
return( cosh(*x) );
}
