#include "f2c.h"

double d_dim(a,b)
doublereal *a, *b;
{
return( *a > *b ? *a - *b : 0);
}
