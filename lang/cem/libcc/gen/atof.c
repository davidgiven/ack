/* $Header$ */
#ifndef NOFLOAT
#include <ctype.h>

extern double ldexp();

double
atof(p)
	register char *p;
{
	register int c;
	int exp = 0, sign = 1, expsign = 0;
	double fl;
	double big = (double)(1L << 30) * (1L << 22);

	while (isspace(*p)) p++;
	c = *p;

	switch (c) {
	case '-':
		sign = -1;
	case '+': 
		p++;
	}

	fl = 0.0;
	while (isdigit(c = *p++)) {
		if (fl < big)
			fl = 10.0 * fl + (double)(c - '0');
		else
			exp++;
	}
	if (c == '.') {
		while (isdigit(c = *p++)) {
			if (fl < big) {
				fl = 10.0 * fl + (double) (c - '0');
				exp--;
			}
		}
	}
	if (fl == 0) return 0;
	if (c == 'E' || c == 'e') {
		int exp1 = 0;
		int sign = 1;

		switch (*p) {
		case '-':
			sign = -1;
		case '+':
			p++;
		}
		while (isdigit(c = *p++)) {
			exp1 = 10 * exp1 + c - '0';
		}
		exp += sign * exp1;
	}

	if (exp < 0) {
		expsign = 1;
		exp = -exp;
	}

	if (exp != 0) {
		int oldexp = exp;
		double exp5 = 5;
		double correction = 1;

		while (exp) {
			if (exp % 2) correction *= exp5;
			exp /= 2;
			exp5 *= exp5;
		}
		if (expsign) fl = fl / correction;
		else	fl = fl * correction;

		fl = ldexp(fl, expsign ? -oldexp : oldexp);
	}

	return sign * fl;
}
#endif
