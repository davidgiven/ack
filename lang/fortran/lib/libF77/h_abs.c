#include "f2c.h"

extern integer s_cmp();

shortint h_abs(x)
shortint *x;
{
if(*x >= 0)
	return(*x);
return(- *x);
}
