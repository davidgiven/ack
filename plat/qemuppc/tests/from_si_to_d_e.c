#include "test.h"

/* Constants in globals to defeat constant folding. */
int one = 1;
int zero = 0;
int minusone = -1;
int big = 0x7fffffff;
int minusbig = -0x8000000;

/* Bypasses the CRT, so there's no stdio or BSS initialisation. */
void _m_a_i_n(void)
{
    ASSERT((double)zero == 0.0);
    ASSERT((double)one == 1.0);
    ASSERT((double)minusone == -1.0);
    ASSERT((double)big == 2147483647.0);
    /* ASSERT((double)minusbig == -2147483648.0); FIXME: fails for now */

    finished();
}