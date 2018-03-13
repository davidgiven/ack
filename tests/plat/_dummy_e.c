#include "test.h"

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT(0 == 0);
    finished();
}
