#include "f2c.h"

VOID pow_ci(p, a, b) 	/* p = a**b  */
complex *p, *a;
integer *b;
{
doublecomplex p1, a1;

a1.r = a->r;
a1.i = a->i;

pow_zi(&p1, &a1, b);

p->r = p1.r;
p->i = p1.i;
}
