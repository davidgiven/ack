/* Rounding to the precision of a target type. */

#include <stdint.h>
#include "flt_misc.h"

void
flt_rnd(flt_arith *e, int bits, int emin)
{
	/*	Round e to bits bits, to nearest, ties to even; below 2^emin
		fewer bits are kept, as in a subnormal number.
		The value is m * 2^(flt_exp - 63), m the 64-bit mantissa.
	*/
	uint64_t m = ((uint64_t)e->m1 << 32) | e->m2;
	uint64_t keep, rest, half;
	int drop;

	if (m == 0)
		return;
	if (e->flt_exp < emin)
		bits -= emin - e->flt_exp;
	drop = 64 - bits;	/* the number of bits to round off */
	if (drop <= 0)
		return;
	if (drop > 64) {
		/* less than half the smallest unit */
		keep = 0;
	}
	else if (drop == 64) {
		/* m is at least half a unit; exactly half rounds to 0 */
		keep = m > ((uint64_t)1 << 63);
	}
	else {
		keep = m >> drop;
		rest = m & (((uint64_t)1 << drop) - 1);
		half = (uint64_t)1 << (drop - 1);
		if (rest > half || (rest == half && (keep & 1)))
			keep++;
	}
	if (keep == 0) {
		e->m1 = e->m2 = 0;
		e->flt_exp = 0;
		e->flt_sign = 0;
		return;
	}
	/* keep units of 2^(flt_exp - 63 + drop): normalize again */
	e->m1 = (uint32_t)(keep >> 32);
	e->m2 = (uint32_t)keep;
	e->flt_exp += drop;
	flt_nrm(e);
	flt_chk(e);
}
