#include "f2c.h"

extern integer s_cmp();

shortint h_sign(a,b)
shortint *a, *b;
{
shortint x;
x = (*a >= 0 ? *a : - *a);
return( *b >= 0 ? x : -x);
}
