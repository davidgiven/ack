/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* $Id$ */

#include <math.h>
#include <errno.h>

extern int errno;
extern double modf(), exp(), log();

double
pow(x,y)
	double x,y;
{
	/*	Simple version for now. The Cody and Waite book has
		a very complicated, much more precise version, but
		this version has machine-dependant arrays A1 and A2,
		and I don't know yet how to solve this ???
	*/
	double dummy;
	int	result_neg = 0;

	if ((x == 0 && y == 0) ||
	    (x < 0 && modf(y, &dummy) != 0)) {
		errno = EDOM;
		return 0;
	}

	if (x == 0) return x;

	if (x < 0) {
		if (modf(y/2.0, &dummy) != 0) {
			/* y was odd */
			result_neg = 1;
		}
		x = -x;
	}
	x = log(x);
	if (x < 0) {
		x = -x;
		y = -y;
	}
	if (y > M_LN_MAX_D/x) {
		errno = ERANGE;
		return 0;
	}
	if (y < M_LN_MIN_D/x) {
		errno = ERANGE;
		return 0;
	}

	x = exp(x * y);
	return result_neg ? -x : x;
}
