#include "f2c.h"

double d_tanh(x)
doublereal *x;
{
double tanh();
return( tanh(*x) );
}
