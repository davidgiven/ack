#include "f2c.h"

double r_sinh(x)
real *x;
{
double sinh();
return( sinh(*x) );
}
