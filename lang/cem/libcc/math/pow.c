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
pow(x,y)
	double x,y;
{
	double dummy;
	extern double modf(), exp(), log();

	if ((x == 0 && y == 0) ||
	    (x < 0 && modf(y, &dummy) != 0)) {
		errno = EDOM;
		return 0;
	}

	if (x == 0) return x;

	if (x < 0) {
		double val = exp(log(-x) * y);
		if (modf(y/2.0, &dummy) != 0) {
			/* y was odd */
			val = - val;
		}
		return val;
	}

	return exp(log(x) * y);
}
