#include "f2c.h"

double r_exp(x)
real *x;
{
double exp();
return( exp(*x) );
}
