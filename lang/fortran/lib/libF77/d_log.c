#include "f2c.h"

double d_log(x)
doublereal *x;
{
double log();
return( log(*x) );
}
