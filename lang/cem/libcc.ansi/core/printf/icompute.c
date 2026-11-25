/*
 * icompute.c - compute an integer
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

extern	void	putstr(const char *s);
extern	void	print(int x);
extern	void	puthexi(int x);
extern	void	puthexl(long x);

/* This routine is used in doprnt.c as well as in tmpfile.c and tmpnam.c. */

char* _i_compute(unsigned long val, int base, char* s, int nrdigits)
{
	int c;

#if 0
putstr("_i_compute: val=");puthexl(val);
putstr("          : base=");print(base);
putstr("          : nrdigits=");print(nrdigits);
putstr("          : s=");putstr(s);putstr("\n");
#endif

	c = val % base;
	val /= base;
	if (val || nrdigits > 1)
		s = _i_compute(val, base, s, nrdigits - 1);
	*s++ = (c > 9 ? c - 10 + 'a' : c + '0');
	return s;
}

#endif
