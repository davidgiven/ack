#include "f2c.h"

double erf_(x)
real *x;
{
double erf();

return( erf(*x) );
}
