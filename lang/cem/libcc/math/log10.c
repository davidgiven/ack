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
log10(x)
	double x;
{
	extern double log();

	if (x <= 0) {
		errno = EDOM;
		return 0;
	}

	return log(x) / M_LN10;
}
