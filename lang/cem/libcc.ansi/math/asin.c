/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
/* $Header$ */

#include	<errno.h>
#include	<math.h>
#include	"localmath.h"


static double
asin_acos(double x, int cosfl)
{
	int negative = x < 0;

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
asin(double x)
{
	return asin_acos(x, 0);
}

double
acos(double x)
{
	return asin_acos(x, 1);
}
