#include "f2c.h"

double d_atn2(x,y)
doublereal *x, *y;
{
double atan2();
return( atan2(*x,*y) );
}
