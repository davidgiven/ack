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
log10(double x)
{
	if (x <= 0) {
		errno = EDOM;
		return 0;
	}

	return log(x) / M_LN10;
}
