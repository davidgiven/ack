#include "f2c.h"

VOID c_exp(r, z)
complex *r, *z;
{
double expx;
double exp(), cos(), sin();

expx = exp(z->r);
r->r = expx * cos(z->i);
r->i = expx * sin(z->i);
}
