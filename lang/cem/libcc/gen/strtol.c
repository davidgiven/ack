/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include	<ctype.h>

long int
strtol(nptr, endptr, base)
	register char	*nptr;
	char		**endptr;
{
	register int v;
	register long val = 0;
	register int c;
	int sign = 1;
	char *startnptr = nptr, *nrstart;

	if (endptr) *endptr = (char *)nptr;
	while (isspace(*nptr)) nptr++;
	c = *nptr;

	if (c == '-' || c == '+') {
		if (c == '-') sign = -1;
		nptr++;
	}
	nrstart = nptr;			/* start of the number */

	/* When base is 0, the syntax determines the actual base */
	if (base == 0)
		if (*nptr == '0')
			if (*++nptr == 'x' || *nptr == 'X') {
				base = 16;
				nptr++;
			}
			else	base = 8;
		else	base = 10;
	else if (base==16 && *nptr=='0' && (*++nptr =='x' || *nptr =='X'))
		nptr++;

	while (isdigit(c = *nptr) || isalpha(c)) {
		if (isalpha(c))
			v = 10 + (isupper(c) ? c - 'A' : c - 'a');
		else
			v = c - '0';
		if (v >= base) break;
		val = (val * base) + v;
		nptr++;
	}
	if (endptr) {
		if (nrstart == nptr) *endptr = (char *)startnptr;
		else *endptr = (char *)nptr;
	}
	return (long) sign * val;
}
