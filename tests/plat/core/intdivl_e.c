#include "test.h"

/* Constants in globals to defeat constant folding. */
long three = 3;
long two = 2;
long one = 1;
long zero = 0;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT(((unsigned long)three / (unsigned long)two) == 1);
    ASSERT((-(unsigned long)three / (unsigned long)two) == ((unsigned long)-3 / (unsigned long)2));
    ASSERT((-(unsigned long)three / -(unsigned long)two) == ((unsigned long)-3 / (unsigned long)-2));
    ASSERT(((unsigned long)three / -(unsigned long)two) == ((unsigned long)3 / (unsigned long)-2));

    ASSERT(((unsigned long)three / (unsigned long)2) == 1);
    ASSERT((-(unsigned long)three / (unsigned long)2) == ((unsigned long)-3 / (unsigned long)2));
    ASSERT((-(unsigned long)three / (unsigned long)-2) == ((unsigned long)-3 / (unsigned long)-2));
    ASSERT(((unsigned long)three / (unsigned long)-2) == ((unsigned long)3 / (unsigned long)-2));

    ASSERT(((unsigned long)3 / (unsigned long)two) == 1);
    ASSERT(((unsigned long)-3 / (unsigned long)two) == ((unsigned long)-3 / (unsigned long)2));
    ASSERT(((unsigned long)-3 / (unsigned long)-two) == ((unsigned long)-3 / (unsigned long)-2));
    ASSERT(((unsigned long)3 / (unsigned long)-two) == ((unsigned long)3 / (unsigned long)-2));

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
