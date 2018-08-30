#include "test.h"

/*
 * Function a() comes from this mail by Rune to tack-devel:
 * https://sourceforge.net/p/tack/mailman/message/35809953/
 *
 * The peephole optimiser (util/opt) had a bug that rewrote
 * xx(! i, i) as xx(1).  It was confused with xx(i == i).
 */

void xx(int xfal, int x1234) {
  ASSERT(xfal == 0);
  ASSERT(x1234 == 1234);
}

void a(void) {
  int i = 1234;
  xx(! i, i);
}

void xxxx(int x2005, int xfal, int xn567, int x2017) {
  ASSERT(x2005 == 2005);
  ASSERT(xfal == 0);
  ASSERT(xn567 == -567);
  ASSERT(x2017 == 2017);
}

/* Like a(), but with surrounding arguments. */
void b(void) {
  int i = -567;
  xxxx(2005, ! i, i, 2017);
}

/*
 * In c(), the fixed peephole optimiser may
 * rewrite i == i as 1 and i != i as 0.
 */
void c(int i, int tru, int fal) {
  ASSERT((i == i) == tru);
  ASSERT((i != i) == fal);
}

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void) {
  a();
  b();
  c(62, 1, 0);
  finished();
}
