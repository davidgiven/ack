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
tanh(double x)
{
	if (x <= 0.5*M_LN_MIN_D) {
		return -1;
	}
	if (x >= 0.5*M_LN_MAX_D) {
		return 1;
	}
	x = exp(x + x);
	return (x - 1.0)/(x + 1.0);
}
