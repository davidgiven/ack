#include "f2c.h"

integer i_dim(a,b)
integer *a, *b;
{
return( *a > *b ? *a - *b : 0);
}
