#include "f2c.h"

#define log10e 0.43429448190325182765

double r_lg10(x)
real *x;
{
double log();

return( log10e * log(*x) );
}
