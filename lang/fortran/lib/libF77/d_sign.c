#include "f2c.h"

double d_sign(a,b)
doublereal *a, *b;
{
double x;
x = (*a >= 0 ? *a : - *a);
return( *b >= 0 ? x : -x);
}
