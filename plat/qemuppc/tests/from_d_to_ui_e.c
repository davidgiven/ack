#include "test.h"

/* Constants in globals to defeat constant folding. */
double one = 1.0;
double zero = 0.0;
double big = 4294967295.0;

/* Bypasses the CRT, so there's no stdio or BSS initialisation. */
void _m_a_i_n(void)
{
    ASSERT((unsigned int)zero == 0);
    ASSERT((unsigned int)one == 1);
    ASSERT((unsigned int)big == 4294967295);

    finished();
}