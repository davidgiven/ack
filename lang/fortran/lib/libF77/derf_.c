#include "f2c.h"

double derf_(x)
doublereal *x;
{
double erf();

return( erf(*x) );
}
