#include "f2c.h"

double d_cos(x)
doublereal *x;
{
double cos();
return( cos(*x) );
}
