#include "f2c.h"

double derfc_(x)
doublereal *x;
{
double erfc();

return( erfc(*x) );
}
