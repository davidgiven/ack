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

#define NITER	5

double
sqrt(x)
	double x;
{
	extern double frexp(), ldexp();
	int exponent;
	double val;

	if (x <= 0) {
		if (x < 0) errno = EDOM;
		return 0;
	}

	val = frexp(x, &exponent);
	if (exponent & 1) {
		exponent--;
		val *= 2;
	}
	val = ldexp(val + 1.0, exponent/2 - 1);
	/* was: val = (val + 1.0)/2.0; val = ldexp(val, exponent/2); */
	for (exponent = NITER - 1; exponent >= 0; exponent--) {
		val = (val + x / val) / 2.0;
	}
	return val;
}
