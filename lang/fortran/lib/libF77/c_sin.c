#include "f2c.h"

VOID c_sin(r, z)
complex *r, *z;
{
double sin(), cos(), sinh(), cosh();

r->r = sin(z->r) * cosh(z->i);
r->i = cos(z->r) * sinh(z->i);
}
