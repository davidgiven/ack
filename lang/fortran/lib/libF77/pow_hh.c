#include "f2c.h"

shortint pow_hh(ap, bp)
shortint *ap, *bp;
{
shortint pow, x, n;

pow = 1;
x = *ap;
n = *bp;

if(n < 0)
	{ }
else if(n > 0)
	for( ; ; )
		{
		if(n & 01)
			pow *= x;
		if(n >>= 1)
			x *= x;
		else
			break;
		}
return(pow);
}
