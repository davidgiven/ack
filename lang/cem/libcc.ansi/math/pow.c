/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
/* $Id$ */

#include	<math.h>
#include	<float.h>
#include	<errno.h>
#include	<limits.h>

double
pow(double x, double y)
{
	double	y_intpart, y_fractpart, fp;
	int	negexp, negx;
	int	ex, newexp;
	unsigned long yi;

	if (x == 1.0) return x;

	if (x == 0 && y <= 0) {
		errno = EDOM;
		return 0;
	}

	if (y == 0) return 1.0;

	if (y < 0) {
		y = -y;
		negexp = 1;
	}
	else	negexp = 0;

	y_fractpart = modf(y, &y_intpart);

	if (y_fractpart != 0) {
		if (x < 0) {
			errno = EDOM;
			return 0;
		}
	}

	negx = 0;
	if (x < 0) {
		x = -x;
		negx = 1;
	}

	if (y_intpart > ULONG_MAX) {
		if (negx && modf(y_intpart/2.0, &y_fractpart) == 0) {
			negx = 0;
		}

		x = log(x);

		/* Beware of overflow in the multiplication */
		if (x > 1.0 && y > DBL_MAX/x) {
			errno = ERANGE;
			return HUGE_VAL;
		}
		if (negexp) y = -y;

		if (negx) return -exp(x*y);
		return exp(x * y);
	}

	if (y_fractpart != 0) {
		fp = exp(y_fractpart * log(x));
	}
	else	fp = 1.0;
	yi = y_intpart;
	if (! (yi & 1)) negx = 0;
	x = frexp(x, &ex);
	newexp = 0;
	for (;;) {
		if (yi & 1) {
			fp *= x;
			newexp += ex;
		}
		yi >>= 1;
		if (yi == 0) break;
		x *= x;
		ex <<= 1;
		if (x < 0.5) {
			x += x;
			ex -= 1;
		}
	}
	if (negexp) {
		fp = 1.0/fp;
		newexp = -newexp;
	}
	if (negx) {
		return -ldexp(fp, newexp);
	}
	return ldexp(fp, newexp);
}
