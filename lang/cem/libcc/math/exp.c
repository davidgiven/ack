/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* $Header$ */

#include <math.h>
#include <errno.h>

extern int errno;

double
exp(x)
	double x;
{
	/*	2**x = (Q(x*x)+x*P(x*x))/(Q(x*x)-x*P(x*x)) for x in [0,0.5] */
	/*	Hart & Cheney #1069 */

	static double p[3] = {
		 0.2080384346694663001443843411e+07,
		 0.3028697169744036299076048876e+05,
		 0.6061485330061080841615584556e+02
	};

	static double q[4] = {
		 0.6002720360238832528230907598e+07,
		 0.3277251518082914423057964422e+06,
		 0.1749287689093076403844945335e+04,
		 0.1000000000000000000000000000e+01
	};

	int negative = x < 0;
	int ipart, large = 0;
	double xsqr, xPxx, Qxx;
	extern double floor(), ldexp();

	if (x <= M_LN_MIN_D) {
		if (x < M_LN_MIN_D) errno = ERANGE;
		return M_MIN_D;
	}
	if (x >= M_LN_MAX_D) {
		if (x > M_LN_MAX_D) errno = ERANGE;
		return M_MAX_D;
	}

	if (negative) {
		x = -x;
	}
	x /= M_LN2;
	ipart = floor(x);
	x -= ipart;
	if (x > 0.5) {
		large = 1;
		x -= 0.5;
	}
	xsqr = x * x;
	xPxx = x * POLYNOM2(xsqr, p);
	Qxx = POLYNOM3(xsqr, q);
	x = (Qxx + xPxx) / (Qxx - xPxx);
	if (large) x *= M_SQRT2;
	x = ldexp(x, ipart);
	if (negative) return 1.0/x;
	return x;
}
