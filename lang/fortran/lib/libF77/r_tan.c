#include "f2c.h"

double r_tan(x)
real *x;
{
double tan();
return( tan(*x) );
}
