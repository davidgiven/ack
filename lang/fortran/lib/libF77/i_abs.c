#include "f2c.h"

integer i_abs(x)
integer *x;
{
if(*x >= 0)
	return(*x);
return(- *x);
}
