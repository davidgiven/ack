#include "test.h"

/* Constants in globals to defeat constant folding. */
double one = 1.0;
double zero = 0.0;

/* Bypasses the CRT, so there's no stdio or BSS initialisation. */
void _m_a_i_n(void)
{
    ASSERT(zero == zero);
    ASSERT(one  != zero);
    ASSERT(zero <  one);
    ASSERT(zero <= one);
    ASSERT(zero <= zero);
    ASSERT(one  >  zero);
    ASSERT(one  >= zero);
    ASSERT(one  >= one);

    finished();
}