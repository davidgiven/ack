#include "f2c.h"

double d_tan(x)
doublereal *x;
{
double tan();
return( tan(*x) );
}
