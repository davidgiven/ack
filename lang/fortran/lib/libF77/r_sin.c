#include "f2c.h"

double r_sin(x)
real *x;
{
double sin();
return( sin(*x) );
}
