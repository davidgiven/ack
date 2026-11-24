/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
/* $Id$ */

#include <math.h>
#include <float.h>
#include <errno.h>
#include <ack/config.h>

#if ACKCONF_WANT_FLOAT

/*
 * Correctly rounded: x = M * 2^k with M an integer of 53 or 54 bits and
 * k even; sqrt(x) = sqrt(M * 2^52) * 2^((k - 52) / 2), and the integer
 * square root of M * 2^52 is computed exactly, two bits at a time, in
 * pairs of 32-bit words. The root r has 53 bits; it is rounded up when
 * the remainder exceeds r (never a tie: sqrt of an integer is an integer
 * or irrational). No byte order is assumed.
 */

#define M32 0xFFFFFFFFUL

struct u64
{
	unsigned long hi, lo; /* 32 bits in each */
};

/* bit i (0..53) of M * 2^52 >> 52, i.e. of M */
static int
mbit(unsigned long hi, unsigned long lo, int i)
{
	return (int)((i < 32 ? lo >> i : hi >> (i - 32)) & 1);
}

double
sqrt(double x)
{
	double m;
	int e, k, i;
	unsigned long mhi, mlo;
	struct u64 rem, root, trial;

	if (__IsNan(x))
	{
		errno = EDOM;
		return x;
	}
	if (x <= 0)
	{
		if (x < 0)
		{
			errno = EDOM;
			return 0;
		}
		return x; /* +0 or -0 */
	}

	if (x > DBL_MAX)
		return x; /* for infinity */

	m = ldexp(frexp(x, &e), 53); /* an integer, 2^52 <= M < 2^53 */
	k = e - 53;		     /* x = M * 2^k */
	mhi = (unsigned long)(m / 4294967296.0);
	mlo = (unsigned long)(m - (double)mhi * 4294967296.0);
	if (k & 1)
	{
		mhi = (mhi << 1) | (mlo >> 31);
		mlo = (mlo << 1) & M32;
		k--;
	}

	rem.hi = rem.lo = root.hi = root.lo = 0;
	/* the bits of M * 2^52, two at a time from bit 105 down */
	for (i = 105; i > 0; i -= 2)
	{
		int two = (i >= 52 ? mbit(mhi, mlo, i - 52) << 1 : 0)
		    | (i - 1 >= 52 ? mbit(mhi, mlo, i - 53) : 0);

		rem.hi = ((rem.hi << 2) | (rem.lo >> 30)) & M32;
		rem.lo = ((rem.lo << 2) | two) & M32;
		trial.hi = ((root.hi << 2) | (root.lo >> 30)) & M32;
		trial.lo = ((root.lo << 2) | 1) & M32;
		root.hi = ((root.hi << 1) | (root.lo >> 31)) & M32;
		root.lo = (root.lo << 1) & M32;
		if (rem.hi > trial.hi || (rem.hi == trial.hi && rem.lo >= trial.lo))
		{
			rem.hi -= trial.hi + (rem.lo < trial.lo);
			rem.lo = (rem.lo - trial.lo) & M32;
			root.lo |= 1;
		}
	}
	/* round to nearest: up when the remainder exceeds the root */
	if (rem.hi > root.hi || (rem.hi == root.hi && rem.lo > root.lo))
	{
		root.lo = (root.lo + 1) & M32;
		if (root.lo == 0)
			root.hi++;
	}
	return ldexp((double)root.hi * 4294967296.0 + (double)root.lo,
	    (k - 52) / 2);
}

#endif

