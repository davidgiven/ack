#include "f2c.h"

double d_atan(x)
doublereal *x;
{
double atan();
return( atan(*x) );
}
