#include "f2c.h"

double r_atn2(x,y)
real *x, *y;
{
double atan2();
return( atan2(*x,*y) );
}
