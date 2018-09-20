#include <limits.h>
#include "test.h"

/* Constants in globals to defeat constant folding. */
double one = 1.0;
double zero = 0.0;
double minusone = -1.0;
double big = (double)INT_MAX;
/* We don't test INT_MIN because some platforms *cough*mips*cough* don't handle
 * it correctly, and we don't want to fail them. */
double minusbig = (double)(INT_MIN+1);

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT((int)zero == 0);
    ASSERT((int)one == 1);
    ASSERT((int)minusone == -1);
    ASSERT((int)big == INT_MAX);
    ASSERT((int)minusbig == (INT_MIN+1));

    finished();
}
