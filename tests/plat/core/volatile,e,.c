#include "test.h"

/* cemcom.ansi emits a nop after each volatile access; every back end must
 * accept it, and a volatile must be read from memory each time. */

volatile char c = 'a';
volatile int i = 1;
volatile long l = 100000L;
volatile unsigned int u = 7;

int count = 3;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    int *p = (int *)&i;
    int n, sum;

    ASSERT(c == 'a');
    ASSERT(i == 1);
    ASSERT(l == 100000L);
    ASSERT(u == 7);

    c = 'b';             ASSERT(c == 'b');
    i = 2;               ASSERT(i == 2);
    l = -200000L;        ASSERT(l == -200000L);
    u = 9;               ASSERT(u == 9);

    c++;                 ASSERT(c == 'c');
    i += 40;             ASSERT(i == 42);
    l *= 2;              ASSERT(l == -400000L);
    u--;                 ASSERT(u == 8);

    /* written behind the compiler's back, then read again */
    i = 5;
    *p = 6;              ASSERT(i == 6);

    /* read in a loop */
    sum = 0;
    for (n = 0; n < count; n++)
    {
        i = n;
        sum += i;
    }
    ASSERT(sum == 3);

    finished();
}
