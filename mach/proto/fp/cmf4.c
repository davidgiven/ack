/*
	COMPARE	DOUBLES
*/

#include	"FP_types.h"
#include	"get_put.h"

short
cmf4(f1,f2)
_float	f1,f2;
{
		/*
		 * return ((f1 < f2) ? 1 : (f1 - f2))
		 */
#define SIGN(x) (((x) < 0) ? -1 : 1)
        int	sign1,sign2;
	long	l1,l2;

	l1 = get4((char *) &f1);
	l2 = get4((char *) &f2);

	if (l1 == l2) return 0;

        sign1 = SIGN(l1);
        sign2 = SIGN(l2);
        if (sign1 != sign2)
                return ((sign1 > 0) ? -1 : 1);

	return (sign1 * ((l1 < l2) ? 1 : -1));
}
