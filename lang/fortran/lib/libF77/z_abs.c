#include "f2c.h"

double z_abs(z)
doublecomplex *z;
{
double cabs();

return( cabs( z->r, z->i ) );
}
