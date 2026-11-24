#include "test.h"

/* Unsigned arithmetic on constants that ego propagates into one
 * instruction wraps around in the word instead of growing. */

uint16_t r16 = 0;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    unsigned int m = (unsigned int)-1;
    unsigned int three = 3;
    unsigned int two = 2;
    uint16_t a = 12933;
    uint16_t b = 20802;
    uint32_t big = 0x10000UL;
    unsigned int ua = 12933;
    unsigned int ub = 20802;

    ASSERT(m * three == (unsigned int)-3);
    ASSERT(m + two == 1);
    ASSERT(three - m == 4);
    ASSERT((m << three) == (unsigned int)-8);
    ASSERT((uint16_t)(a * b) == 6986);
    r16 = 100 / (uint16_t)(a * b);
    ASSERT(r16 == 0);
    ASSERT(big * big == 0);
    ASSERT((big << 16) == 0);
    /* Shifting the result of a fold must not see bits beyond the word. */
    ASSERT((uint16_t)((uint16_t)(ua * ub) >> 8) == 27);
    ASSERT((uint32_t)(big * big) >> 16 == 0);
    ASSERT((uint32_t)(big << 16) >> 8 == 0);
    ASSERT((m * three) >> 1 == (unsigned int)-3 >> 1);

    finished();
}
