/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>

div_t
div(register int numer, register int denom)
{
	div_t r;

	r.quot = numer / denom;
	r.rem = numer % denom;
	return r;
}
