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

double
yn(n, x)
	double x;
{
	/*	Use y0, y1, and the recurrence relation
		y(n+1,x) = 2*n*y(n,x)/x - y(n-1, x).
		According to Hart & Cheney, this is stable for all
		x, n.
		Also use: y(-n,x) = (-1)^n * y(n, x)
	*/

	int negative = 0;
	extern double y0(), y1();
	double yn1, yn2;
	register int i;

	if (x <= 0) {
		errno = EDOM;
		return -HUGE;
	}

	if (n < 0) {
		n = -n;
		negative = (n % 2);
	}

	if (n == 0) return y0(x);
	if (n == 1) return y1(x);

	yn2 = y0(x);
	yn1 = y1(x);
	for (i = 1; i < n; i++) {
		double tmp = yn1;
		yn1 = (i*2)*yn1/x - yn2;
		yn2 = tmp;
	}
	if (negative) return -yn1;
	return yn1;
}

double
jn(n, x)
	double x;
{
	/*	Unfortunately, according to Hart & Cheney, the recurrence
		j(n+1,x) = 2*n*j(n,x)/x - j(n-1,x) is unstable for
		increasing n, except when x > n.
		However, j(n,x)/j(n-1,x) = 2/(2*n-x*x/(2*(n+1)-x*x/( .... 
		(a continued fraction).
		We can use this to determine KJn and KJn-1, where K is a
		normalization constant not yet known. This enables us
		to determine KJn-2, ...., KJ1, KJ0. Now we can use the
		J0 or J1 approximation to determine K.
		Use: j(-n, x) = (-1)^n * j(n, x)
		     j(n, -x) = (-1)^n * j(n, x)
	*/

	extern double j0(), j1();

	if (n < 0) {
		n = -n;
		x = -x;
	}

	if (n == 0) return j0(x);
	if (n == 1) return j1(x);
	if (x > n) {
		/* in this case, the recurrence relation is stable for
		   increasing n, so we use that.
		*/
		double jn2 = j0(x), jn1 = j1(x);
		register int i;

		for (i = 1; i < n; i++) {
			double tmp = jn1;
			jn1 = (2*i)*jn1/x - jn2;
			jn2 = tmp;
		}
		return jn1;
	}
	{
		/* we first compute j(n,x)/j(n-1,x) */
		register int i;
		double quotient = 0.0;
		double xsqr = x*x;
		double jn1, jn2;

		for (i = 20;	/* ??? how many do we need ??? */
		     i > 0; i--) {
			quotient = xsqr/(2*(i+n) - quotient);
		}
		quotient = x / (2*n - quotient);

		jn1 = quotient;
		jn2 = 1.0;
		for (i = n-1; i > 0; i--) {
			/* recurrence relation is stable for decreasing n
			*/
			double tmp = jn2;
			jn2 = (2*i)*jn2/x - jn1;
			jn1 = tmp;
		}
		/* So, now we have K*Jn = quotient and K*J0 = jn2.
		   Now it is easy; compute real j0, this gives K = jn2/j0,
		   and this then gives Jn = quotient/K = j0 * quotient / jn2.
		*/
		return j0(x)*quotient/jn2;
	}
}
