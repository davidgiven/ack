#include "f2c.h"

extern integer s_cmp();

shortint h_dim(a,b)
shortint *a, *b;
{
return( *a > *b ? *a - *b : 0);
}
