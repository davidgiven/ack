#include "f2c.h"

double pow_dd(ap, bp)
doublereal *ap, *bp;
{
double pow();

return(pow(*ap, *bp) );
}
