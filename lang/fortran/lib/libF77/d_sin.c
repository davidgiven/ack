#include "f2c.h"

double d_sin(x)
doublereal *x;
{
double sin();
return( sin(*x) );
}
