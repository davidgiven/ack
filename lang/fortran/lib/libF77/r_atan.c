#include "f2c.h"

double r_atan(x)
real *x;
{
double atan();
return( atan(*x) );
}
