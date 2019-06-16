/*
 * icompute.c - compute an integer
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

/* This routine is used in doprnt.c as well as in tmpfile.c and tmpnam.c. */

char* _i_compute(unsigned long val, int base, char* s, int nrdigits)
{
	int c;

	c = val % base;
	val /= base;
	if (val || nrdigits > 1)
		s = _i_compute(val, base, s, nrdigits - 1);
	*s++ = (c > 9 ? c - 10 + 'a' : c + '0');
	return s;
}

#endif
