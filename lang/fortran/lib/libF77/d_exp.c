#include "f2c.h"

double d_exp(x)
doublereal *x;
{
double exp();
return( exp(*x) );
}
