#include "test.h"

/* x & (a op b) is x & 0 or x & 1, not "x with bit 0 cleared if false". */

int16_t one = 1;
int16_t two = 2;
int8_t g = 0x40;
int32_t sh = 0;
int ione = 1, itwo = 2, ish = 0;
long lone = 1, ltwo = 2, lsh = 0;

static int16_t f(void)
{
    return (int16_t)((int32_t)-8017 >> ((uint8_t)sh & 31))
        & (int16_t)((int32_t)-1L && (int16_t)g);
}

static int16_t h(int16_t x)
{
    return (int16_t)((int32_t)x >> ((uint8_t)sh & 31)) & (int16_t)(one < two);
}

static int16_t k(int16_t x)
{
    return (int16_t)((int32_t)x >> ((uint8_t)sh & 31)) & (int16_t)(one == two);
}

/* The same with int and long operands, for every word size. */

static int ih(int x)
{
    return (x >> ish) & (ione < itwo);
}

static int ik(int x)
{
    return (x >> ish) & (ione == itwo);
}

static int ic(int x, int a, int b)
{
    return (x >> ish) & (a < b);
}

static long lh(long x)
{
    return (x >> lsh) & (lone < ltwo);
}

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT(f() == 1);
    ASSERT(h(-8017) == 1);
    ASSERT(h(-8018) == 0);
    ASSERT(k(-8017) == 0);
    ASSERT(ih(-8017) == 1);
    ASSERT(ih(-8018) == 0);
    ASSERT(ik(-8017) == 0);
    ASSERT(ic(-8017, 1, 2) == 1);
    ASSERT(ic(-8017, 2, 1) == 0);
    ASSERT(lh(-8017L) == 1);

    finished();
}
