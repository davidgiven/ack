#include "f2c.h"

double c_abs(z)
complex *z;
{
double cabs();

return( cabs( z->r, z->i ) );
}
