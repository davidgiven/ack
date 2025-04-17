#include <limits.h>
#include "test.h"

/* Constants in globals to defeat constant folding. */
int two = 2;
int one = 1;
int zero = 0;
int minusone = -1;
int biggish = 258;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT((two - one) == 1);
    ASSERT((one - two) == -1);

    ASSERT((two - 1) == 1);
    ASSERT((one - 2) == -1);

    ASSERT((2   - one) == 1);
    ASSERT((1   - two) == -1);

    ASSERT(((unsigned int)two - (unsigned int)one) == 1);
    ASSERT(((unsigned int)one - (unsigned int)two) == UINT_MAX);

    ASSERT(((unsigned int)two - (unsigned int)1) == 1);
    ASSERT(((unsigned int)one - (unsigned int)2) == UINT_MAX);

    ASSERT(((unsigned int)2   - (unsigned int)one) == 1);
    ASSERT(((unsigned int)1   - (unsigned int)two) == UINT_MAX);

    ASSERT(((unsigned int)biggish - (unsigned int)256) == 2);

    finished();
}
