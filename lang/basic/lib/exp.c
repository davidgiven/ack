/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* $Id$ */

#define __NO_DEFS
#include <math.h>

static double
ldexp(fl,exp)
	double fl;
	int exp;
{
	extern double _fef();
	int sign = 1;
	int currexp;

	if (fl<0) {
		fl = -fl;
		sign = -1;
	}
	fl = _fef(fl,&currexp);
	exp += currexp;
	if (exp > 0) {
		while (exp>30) {
			fl *= (double) (1L << 30);
			exp -= 30;
		}
		fl *= (double) (1L << exp);
	}
	else	{
		while (exp<-30) {
			fl /= (double) (1L << 30);
			exp += 30;
		}
		fl /= (double) (1L << -exp);
	}
	return sign * fl;
}

double
_exp(x)
	double	x;
{
	/*	Algorithm and coefficients from:
			"Software manual for the elementary functions"
			by W.J. Cody and W. Waite, Prentice-Hall, 1980
	*/

	static double p[] = {
	        0.25000000000000000000e+0,
	        0.75753180159422776666e-2,
		0.31555192765684646356e-4
	};

	static double q[] = {
	        0.50000000000000000000e+0,
	        0.56817302698551221787e-1,
	        0.63121894374398503557e-3,
		0.75104028399870046114e-6
	};
	double	xn, g;
	int	n;
	int	negative = x < 0;

	if (x <= M_LN_MIN_D) {
		return M_MIN_D;
	}
	if (x >= M_LN_MAX_D) {
		if (x > M_LN_MAX_D) error(3);
		return M_MAX_D;
	}
	if (negative) x = -x;

	/* ??? avoid underflow ??? */

	n = x * M_LOG2E + 0.5;	/* 1/ln(2) = log2(e), 0.5 added for rounding */
	xn = n;
	{
		double	x1 = (long) x;
		double	x2 = x - x1;

		g = ((x1-xn*0.693359375)+x2) - xn*(-2.1219444005469058277e-4);
	}
	if (negative) {
		g = -g;
		n = -n;
	}
	xn = g * g;
	x = g * POLYNOM2(xn, p);
	n += 1;
	return (ldexp(0.5 + x/(POLYNOM3(xn, q) - x), n));
}
