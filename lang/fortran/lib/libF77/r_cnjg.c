#include "f2c.h"

VOID r_cnjg(r, z)
complex *r, *z;
{
r->r = z->r;
r->i = - z->i;
}
