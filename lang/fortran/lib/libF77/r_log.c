#include "f2c.h"

double r_log(x)
real *x;
{
double log();
return( log(*x) );
}
