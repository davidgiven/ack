#include <math.h>
#include "lib.h"

/* base ^ exponent. The compiler pushes the base first, so the exponent is
 * the first parameter (the original computed exp(first * log(second))). */
double _power(double exponent, double base) { return pow(base, exponent); }
