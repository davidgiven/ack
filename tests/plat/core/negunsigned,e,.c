#include "test.h"

/* Unary - and ~ of an unsigned constant are folded modulo the type's
 * size, also when the compiler's own arithmetic is wider. */

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT(-(uint32_t)1371337135UL / 193754694UL == 15);
    ASSERT(-(uint32_t)1 == 0xFFFFFFFFUL);
    ASSERT((-(uint32_t)1 >> 31) == 1);
    ASSERT(~(uint32_t)0 / 0x10000UL == 0xFFFFUL);
    ASSERT(-(unsigned int)1 / 2 == (unsigned int)-1 / 2);

    finished();
}
