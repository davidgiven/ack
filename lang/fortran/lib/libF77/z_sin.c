#include "f2c.h"

VOID z_sin(r, z)
doublecomplex *r, *z;
{
double sin(), cos(), sinh(), cosh();

r->r = sin(z->r) * cosh(z->i);
r->i = cos(z->r) * sinh(z->i);
}
