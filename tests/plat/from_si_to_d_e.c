#include <limits.h>
#include "test.h"

/* Constants in globals to defeat constant folding. */
int one = 1;
int zero = 0;
int minusone = -1;
int big = INT_MAX;
int minusbig = INT_MIN;

/* Bypasses the CRT, so there's no stdio or BSS initialisation. */
void _m_a_i_n(void)
{
    ASSERT((double)zero == 0.0);
    ASSERT((double)one == 1.0);
    ASSERT((double)minusone == -1.0);
    ASSERT((double)big == (double)INT_MAX);
    /* ASSERT((double)minusbig == (double)INT_MIN); FIXME: fails for now */

    finished();
}