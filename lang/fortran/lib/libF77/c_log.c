#include "f2c.h"

VOID c_log(r, z)
complex *r, *z;
{
double log(), cabs(), atan2();

r->i = atan2(z->i, z->r);
r->r = log( cabs(z->r, z->i) );
}
