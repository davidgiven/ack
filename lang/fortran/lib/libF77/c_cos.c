#include "f2c.h"

VOID c_cos(r, z)
complex *r, *z;
{
double sin(), cos(), sinh(), cosh();

r->r = cos(z->r) * cosh(z->i);
r->i = - sin(z->r) * sinh(z->i);
}
