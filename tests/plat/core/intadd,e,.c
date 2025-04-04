#include "test.h"

/* Constants in globals to defeat constant folding. */
int two = 2;
int one = 1;
int zero = 0;
int minusone = -1;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT((two + one)      == 3);
    ASSERT((two + minusone) == 1);

    ASSERT((two +  1) == 3);
    ASSERT((two + -1) == 1);

    ASSERT((1   + two) == 3);
    ASSERT((-1  + two) == 1);

    ASSERT(((unsigned int)two + (unsigned int)one)      == 3);
    ASSERT(((unsigned int)two + (unsigned int)minusone) == 1);

    ASSERT(((unsigned int)two + (unsigned int) 1) == 3);
    ASSERT(((unsigned int)two + (unsigned int)-1) == 1);

    ASSERT(((unsigned int)1   + (unsigned int)two) == 3);
    ASSERT(((unsigned int)-1  + (unsigned int)two) == 1);

    ASSERT(((unsigned int)two + (unsigned int)256) == 258);

    finished();
}
