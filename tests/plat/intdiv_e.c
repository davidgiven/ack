#include "test.h"

/* Constants in globals to defeat constant folding. */
int three = 3;
int two = 2;
int one = 1;
int zero = 0;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
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
