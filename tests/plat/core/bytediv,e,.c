#include "test.h"

/* A byte moved into a cleared register sets the condition codes for the
 * byte, not the register: dividing by a power of two must not take the
 * byte's sign as the register's. */

int16_t l = -300;
int8_t s = -100;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    uint8_t u = (uint8_t)l;

    ASSERT((uint8_t)l == 212);
    ASSERT((uint8_t)l / 16 == 13);
    ASSERT(u / 16 == 13);
    ASSERT(u / 4 == 53);
    ASSERT((uint8_t)s / 2 == 78);
    ASSERT(s / 2 == -50);

    finished();
}
