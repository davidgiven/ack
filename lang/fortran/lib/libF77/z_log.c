#include "f2c.h"

VOID z_log(r, z)
doublecomplex *r, *z;
{
double log(), cabs(), atan2();

r->i = atan2(z->i, z->r);
r->r = log( cabs( z->r, z->i ) );
}
