#include "test.h"

/* Floating constants folded by the compiler must have the value the target
 * computes: each operand rounded to its type, each result rounded to its
 * type, to nearest and ties to even (IEEE). */

/* Operands in globals to defeat constant folding. (Not volatile: the
 * powerpc ncg table has no pattern for the nop cemcom.ansi emits after
 * a volatile access.) */
double d243 = 2.43, d622 = 622.780, d483 = 483.667, d807 = 807.214;
double d866 = 866.963, d603 = 603.873, d975 = 975.296, d16 = 16.764;
double dp1 = 0.1, dp2 = 0.2, d3 = 3.0;
float f243 = 2.43f, f622 = 622.780f, f24 = 24.41f, f58 = 58.848f;
float f975 = 975.296f, f16 = 16.764f, fp1 = 0.1f;

/* The folded values, stored so that comparing them is not folded. */
double d;
float f;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    /* folded as computed at run time; these were 1 ulp off */
    d = 0.1 + 0.2;           ASSERT(d == dp1 + dp2);
    d = 2.43 + 622.780;      ASSERT(d == d243 + d622);
    d = 975.296 + 16.764;    ASSERT(d == d975 + d16);
    d = 866.963 - 603.873;   ASSERT(d == d866 - d603);
    d = 2.43 * 622.780;      ASSERT(d == d243 * d622);
    d = 2.43 / 622.780;      ASSERT(d == d243 / d622);
    d = 483.667 / 807.214;   ASSERT(d == d483 / d807);
    f = 24.41f + 58.848f;    ASSERT(f == f24 + f58);
    f = 975.296f - 16.764f;  ASSERT(f == f975 - f16);
    f = 2.43f * 622.780f;    ASSERT(f == f243 * f622);
    f = 2.43f / 622.780f;    ASSERT(f == f243 / f622);

    /* a float constant has float precision */
    d = (double)0.1f;        ASSERT(d == (double)fp1);
    d = 0.1f * 3.0;          ASSERT(d == fp1 * d3);
    d = 0.1f == 0.1;         ASSERT(d == 0.0);
    d = 0.5f == 0.5;         ASSERT(d == 1.0);

    /* exact values */
    d = 0.1 + 0.2;           ASSERT(d == 0.30000000000000004);
                             ASSERT(d != 0.3);

    /* ties round to even */
    d = 9007199254740993.0;  ASSERT(d == 9007199254740992.0);
    d = 9007199254740995.0;  ASSERT(d == 9007199254740996.0);
    f = 16777217.0f;         ASSERT(f == 16777216.0f);
    f = (float)16777217L;    ASSERT(f == 16777216.0f);
    f = (float)16777219L;    ASSERT(f == 16777220.0f);

    /* subnormal results: half the smallest one is a tie, to 0 */
    d = 4.9406564584124654e-324 / 2.0;  ASSERT(d == 0.0);
    d = 4.9406564584124654e-324 * 1.5;  ASSERT(d == 9.8813129168249309e-324);
    d = 2.2250738585072014e-308 / 4.0;  ASSERT(d == 5.5626846462680035e-309);

    finished();
}
