#include "f2c.h"

d_cnjg(r, z)
doublecomplex *r, *z;
{
r->r = z->r;
r->i = - z->i;
}
