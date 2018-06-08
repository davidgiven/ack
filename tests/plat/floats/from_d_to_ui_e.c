#include <limits.h>
#include "test.h"

/* Constants in globals to defeat constant folding. */
double one = 1.0;
double zero = 0.0;
double big = (double)UINT_MAX;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT((unsigned int)zero == 0);
    ASSERT((unsigned int)one == 1);
    ASSERT((unsigned int)big == UINT_MAX);

    finished();
}
