/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef	NOFLOAT
#include	<ctype.h>
#include	<limits.h>
#include	<math.h>
#include	<stdlib.h>

#define	MAX	(LONG_MAX/10)

double
strtod(register const char *p, register char **pp)
{
	register int c;
	int exp = 0, sign = 1, expsign = 0;
	double fl;
	long lowl = 0, highl = 0, pos = 1;
	int dotseen = 0;
	int digit_seen = 0;

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

	while (isdigit(c = *p++) || (c == '.' && ! dotseen++)) {
		if (c == '.') continue;
		digit_seen = 1;
		if (highl < MAX) {
			highl = (highl << 3) + (highl << 1) + (c - '0');
		}
		else if (pos < MAX) {
			pos = (pos << 3) + (pos << 1);
			lowl = (lowl << 3) + (lowl << 1) + (c - '0');
		}
		else exp++;
		if (dotseen) exp--;
	}
	if (! digit_seen) return 0.0;
	fl = highl;
	if (pos > 1) {
		fl = pos * fl + lowl;
	}

	if (pp) *pp = p-1;

	if (c == 'E' || c == 'e') {
		int exp1 = 0;
		int sign = 1;

		switch (*p) {
		case '-':
			sign = -1;
			/* fallthrough */
		case '+':
			p++;
		}
		if (isdigit(c = *p)) {
			do {
				exp1 = 10 * exp1 + c - '0';
			} while (isdigit(c = *++p));
			if (pp) *pp = p;
		}
		exp += sign * exp1;
	}

	if (fl == 0.0) return 0.0;

	if (exp < 0) {
		expsign = 1;
		exp = -exp;
	}

	if (exp != 0) {
                int oldexp = exp;
                double exp5 = 5.0;
                double correction = 1.0;
 
                while (exp) {
                        if (exp % 2) correction *= exp5;
                        exp /= 2;
                        if (exp != 0) exp5 *= exp5;
                }
                if (expsign) fl = fl / correction;
                else    fl = fl * correction;
 
                fl = ldexp(fl, expsign ? -oldexp : oldexp);
	}

	return sign * fl;
}
#endif
