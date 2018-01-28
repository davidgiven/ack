#include "test.h"

int csa(int i)
{
    switch (i)
    {
        case 200:  return 200;
        case 300:  return 300;
        case 400:  return 400;
        default: return 0;
    }
}

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT(csa(0) == 0);
    ASSERT(csa(100) == 0);
    ASSERT(csa(200) == 200);
    ASSERT(csa(300) == 300);
    ASSERT(csa(400) == 400);
    ASSERT(csa(500) == 0);
    ASSERT(csa(600) == 0);

    finished();
}
