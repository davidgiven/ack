#include "f2c.h"

integer i_sign(a,b)
integer *a, *b;
{
integer x;
x = (*a >= 0 ? *a : - *a);
return( *b >= 0 ? x : -x);
}
