#include "f2c.h"

double d_sinh(x)
doublereal *x;
{
double sinh();
return( sinh(*x) );
}
