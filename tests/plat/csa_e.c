#include "test.h"

int csa(int i)
{
    switch (i)
    {
        case 2:  return 2;
        case 3:  return 3;
        case 4:  return 4;
        default: return 0;
    }
}

/* Bypasses the CRT, so there's no stdio or BSS initialisation. */
void _m_a_i_n(void)
{
    ASSERT(csa(0) == 0);
    ASSERT(csa(1) == 0);
    ASSERT(csa(2) == 2);
    ASSERT(csa(3) == 3);
    ASSERT(csa(4) == 4);
    ASSERT(csa(5) == 0);
    ASSERT(csa(6) == 0);

    finished();
}