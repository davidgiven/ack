#include "f2c.h"

double r_tanh(x)
real *x;
{
double tanh();
return( tanh(*x) );
}
