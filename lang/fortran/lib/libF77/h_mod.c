#include "f2c.h"

extern integer s_cmp();

shortint h_mod(a,b)
short *a, *b;
{
return( *a % *b);
}
