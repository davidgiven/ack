#include "f2c.h"

double d_cosh(x)
doublereal *x;
{
double cosh();
return( cosh(*x) );
}
