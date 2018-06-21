/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>

div_t div(register int numer, register int denom)
{
	div_t r;

	r.quot = numer / denom; /* might trap if denom == 0 */
	r.rem = numer % denom;

	if (r.rem != 0 && (numer > 0) != (r.rem > 0))
	{
		r.quot++;
		r.rem -= denom;
	}

	return r;
}
