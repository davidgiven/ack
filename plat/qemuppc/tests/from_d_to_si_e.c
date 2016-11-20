#include "test.h"

/* Constants in globals to defeat constant folding. */
double one = 1.0;
double zero = 0.0;
double minusone = -1.0;
double big = 2147483647.0;
double minusbig = -2147483648.0;

/* Bypasses the CRT, so there's no stdio or BSS initialisation. */
void _m_a_i_n(void)
{
    ASSERT((int)zero == 0);
    ASSERT((int)one == 1);
    ASSERT((int)minusone == -1);
    ASSERT((int)big == 2147483647);
    ASSERT((int)minusbig == -2147483648);

    finished();
}