/*
 * gcvt.c - conversion for printing a floating point number
 */
/* $Header$ */

#ifndef NOFLOAT
#include	"loc_incl.h"

#define	NDIGINEXP(exp)		(((exp) >= 100 || (exp) <= -100) ? 3 : 2)
#define	LOW_EXP			-4
#define	USE_EXP(exp, ndigits)	(((exp) < LOW_EXP + 1) || (exp >= ndigits + 1))

char *
gcvt(long double value, int ndigit, char *s, int flags)
{
	int sign, dp;
	register char *s1, *s2;
	register int i;
	register int nndigit = ndigit;

	s1 = ecvt(value, ndigit, &dp, &sign);
	s2 = s;
	if (sign) *s2++ = '-';
	else if (flags & FL_SIGN)
		*s2++ = '+';
	else if (flags & FL_SPACE)
		*s2++ = ' ';

	if (!(flags & FL_ALT))
		for (i = nndigit - 1; i > 0 && s1[i] == '0'; i--)
			nndigit--;

	if (USE_EXP(dp,ndigit))	{
		/* Use E format */
		dp--;
		*s2++ = *s1++;
		if ((nndigit > 1) || (flags & FL_ALT)) *s2++ = '.';
		while (--nndigit > 0) *s2++ = *s1++;
		*s2++ = 'e';
		if (dp < 0) {
			*s2++ = '-';
			dp = -dp;
		}
		else	 *s2++ = '+';
		s2 += NDIGINEXP(dp);
		*s2 = 0;
		for (i = NDIGINEXP(dp); i > 0; i--) {
			*--s2 = dp % 10 + '0';
			dp /= 10;
		}
		return s;
	}
	/* Use f format */
	if (dp <= 0) {
		if (*s1 != '0')	{
			/* otherwise the whole number is 0 */
			*s2++ = '0';
			*s2++ = '.';
		}
		while (dp < 0) {
			dp++;
			*s2++ = '0';
		}
	}
	for (i = 1; i <= nndigit; i++) {
		*s2++ = *s1++;
		if (i == dp) *s2++ = '.';
	}
	if (i <= dp) {
		while (i++ <= dp) *s2++ = '0';
		*s2++ = '.';
	}
	if ((s2[-1]=='.') && !(flags & FL_ALT)) s2--;
	*s2 = '\0';
	return s;
}
#endif	/* NOFLOAT */
