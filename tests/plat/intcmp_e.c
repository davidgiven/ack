#include "test.h"

/* Constants in globals to defeat constant folding. */
int one = 1;
int zero = 0;

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

    ASSERT(zero == 0);
    ASSERT(one  != 0);
    ASSERT(zero <  1);
    ASSERT(zero <= 1);
    ASSERT(zero <= 0);
    ASSERT(one  >  0);
    ASSERT(one  >= 0);
    ASSERT(one  >= 1);

    ASSERT(0 == zero);
    ASSERT(1 != zero);
    ASSERT(0 <  one);
    ASSERT(0 <= one);
    ASSERT(0 <= zero);
    ASSERT(1 >  zero);
    ASSERT(1 >= zero);
    ASSERT(1 >= one);

    ASSERT((unsigned int)zero == (unsigned int)zero);
    ASSERT((unsigned int)one  != (unsigned int)zero);
    ASSERT((unsigned int)zero <  (unsigned int)one);
    ASSERT((unsigned int)zero <= (unsigned int)one);
    ASSERT((unsigned int)zero <= (unsigned int)zero);
    ASSERT((unsigned int)one  >  (unsigned int)zero);
    ASSERT((unsigned int)one  >= (unsigned int)zero);
    ASSERT((unsigned int)one  >= (unsigned int)one);

    ASSERT((unsigned int)zero == (unsigned int)0);
    ASSERT((unsigned int)one  != (unsigned int)0);
    ASSERT((unsigned int)zero <  (unsigned int)1);
    ASSERT((unsigned int)zero <= (unsigned int)1);
    ASSERT((unsigned int)zero <= (unsigned int)0);
    ASSERT((unsigned int)one  >  (unsigned int)0);
    ASSERT((unsigned int)one  >= (unsigned int)0);
    ASSERT((unsigned int)one  >= (unsigned int)1);

    ASSERT((unsigned int)0 == (unsigned int)zero);
    ASSERT((unsigned int)1 != (unsigned int)zero);
    ASSERT((unsigned int)0 <  (unsigned int)one);
    ASSERT((unsigned int)0 <= (unsigned int)one);
    ASSERT((unsigned int)0 <= (unsigned int)zero);
    ASSERT((unsigned int)1 >  (unsigned int)zero);
    ASSERT((unsigned int)1 >= (unsigned int)zero);
    ASSERT((unsigned int)1 >= (unsigned int)one);

    finished();
}