/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>
#include	<ctype.h>

long int
strtol(register const char *p, char **pp, int base)
{
	register long val, v;
	register int c;
	int sign = 1;

	if (pp) *pp = p;
	while (isspace(*p)) p++;
	c = *p;

	switch (c) {
	case '-':
		sign = -1;
		/* fallthrough */
	case '+':
		p++;
	}

	/* this is bizare */
	if (base==16 && *p=='0' && (*(p+1)=='x' || *(p+1)=='X'))
		p += 2;

	while (isdigit(c = *p++) || isalpha(c)) {
		if (isalpha(c))
			v = 10 + (isupper(c) ? c - 'A' : c - 'a');
		else
			v = c - '0';
		if (v >= base) {
			p--;
			break;
		}
		val = (val * base) + v;
	}
	if (pp) *pp = p-1;
	return sign * val;
}


unsigned long int
strtoul(register const char *p, char **pp, int base)
{
	return (unsigned long)strtol(p, pp, base);
}
