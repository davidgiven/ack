#include "test.h"

/* Multiplication by a constant with all bits set is not a shift. */

uint32_t x = 3;
uint16_t y = 3;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT((uint32_t)(x * 0xFFFFFFFFUL) == 0xFFFFFFFDUL);
    ASSERT((uint32_t)(x * 0x80000000UL) == 0x80000000UL);
    ASSERT((uint16_t)(y * 0xFFFFU) == 0xFFFDU);

    finished();
}
