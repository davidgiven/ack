#include "f2c.h"

VOID z_cos(r, z)
doublecomplex *r, *z;
{
double sin(), cos(), sinh(), cosh();

r->r = cos(z->r) * cosh(z->i);
r->i = - sin(z->r) * sinh(z->i);
}
