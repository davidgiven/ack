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


static double
asin_acos(x, cosfl)
	double x;
{
	int negative = x < 0;
	extern double sqrt(), atan();

	if (negative) {
		x = -x;
	}
	if (x > 1) {
		errno = EDOM;
		return 0;
	}
	if (x == 1) {
		x = M_PI_2;
	}
	else x = atan(x/sqrt(1-x*x));
	if (negative) x = -x;
	if (cosfl) {
		return M_PI_2 - x;
	}
	return x;
}

double
asin(x)
	double x;
{
	return asin_acos(x, 0);
}

double
acos(x)
	double x;
{
	return asin_acos(x, 1);
}
