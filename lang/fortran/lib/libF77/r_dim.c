#include "f2c.h"

double r_dim(a,b)
real *a, *b;
{
return( *a > *b ? *a - *b : 0);
}
