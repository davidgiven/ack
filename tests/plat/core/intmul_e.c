#include "test.h"

/* Constants in globals to defeat constant folding. */
int two = 2;
int one = 1;
int zero = 0;
int minusone = -1;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT((two * one)      == 2);
    ASSERT((two * minusone) == -2);

    ASSERT((two *  1) == 2);
    ASSERT((two * -1) == -2);

    ASSERT((3   * two) == 6);
    ASSERT((-3  * two) == -6);

    ASSERT(((unsigned int)two * (unsigned int)one)      == 2);
    ASSERT(((unsigned int)two * (unsigned int)minusone) == -2);

    ASSERT(((unsigned int)two * (unsigned int) 3) == 6);
    ASSERT(((unsigned int)two * (unsigned int)-3) == -6);

    ASSERT(((unsigned int)3   * (unsigned int)two) == 6);
    ASSERT(((unsigned int)-3  * (unsigned int)two) == -6);

    ASSERT(((unsigned int)two * (unsigned int)256) == 512);

    finished();
}

