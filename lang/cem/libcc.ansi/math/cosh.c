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

double
cosh(double x)
{
	if (x < 0) {
		x = -x;
	}
	if (x > M_LN_MAX_D) {
		/* exp(x) would overflow */
		if (x >= M_LN_MAX_D + M_LN2) {
			/* not representable */
			x = HUGE_VAL;
			errno = ERANGE;
		}
		else	x = exp (x - M_LN2);
	}
	else {
		double expx = exp(x);
		x = 0.5 * (expx + 1.0/expx);
	}
	return x;
}
