#include "test.h"

/* Constants in globals to defeat constant folding. */
int three = 3;
int two = 2;
int one = 1;
int zero = 0;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT(((unsigned)three / (unsigned)two) == 1);
    ASSERT((-(unsigned)three / (unsigned)two) == ((unsigned)-3 / (unsigned)2));
    ASSERT((-(unsigned)three / -(unsigned)two) == ((unsigned)-3 / (unsigned)-2));
    ASSERT(((unsigned)three / -(unsigned)two) == ((unsigned)3 / (unsigned)-2));

    ASSERT(((unsigned)three / (unsigned)2) == 1);
    ASSERT((-(unsigned)three / (unsigned)2) == ((unsigned)-3 / (unsigned)2));
    ASSERT((-(unsigned)three / (unsigned)-2) == ((unsigned)-3 / (unsigned)-2));
    ASSERT(((unsigned)three / (unsigned)-2) == ((unsigned)3 / (unsigned)-2));

    ASSERT(((unsigned)3 / (unsigned)two) == 1);
    ASSERT(((unsigned)-3 / (unsigned)two) == ((unsigned)-3 / (unsigned)2));
    ASSERT(((unsigned)-3 / (unsigned)-two) == ((unsigned)-3 / (unsigned)-2));
    ASSERT(((unsigned)3 / (unsigned)-two) == ((unsigned)3 / (unsigned)-2));

    ASSERT((three / two) == 1);
    ASSERT((-three / two) == -1);
    ASSERT((-three / -two) == 1);
    ASSERT((three / -two) == -1);

    ASSERT((three / 2) == 1);
    ASSERT((-three / 2) == -1);
    ASSERT((-three / -2) == 1);
    ASSERT((three / -2) == -1);

    ASSERT((3 / two) == 1);
    ASSERT((-3 / two) == -1);
    ASSERT((-3 / -two) == 1);
    ASSERT((3 / -two) == -1);

    finished();
}
