#include "f2c.h"

double r_cos(x)
real *x;
{
double cos();
return( cos(*x) );
}
