/* $Id$ */
#ifndef NOFLOAT
extern char	*ecvt();

#define NDIGINEXP(exp)	(((exp) >= 100 || (exp) <= -100) ? 3 : 2)

char *
gcvt(value, ndigit, buf)
	double value;
	char *buf;
	int ndigit;
{
	int sign, dp;
	register char *s1, *s2;
	register int i;
	register int nndigit = ndigit;

	s1 = ecvt(value, ndigit, &dp, &sign);
	s2 = buf;
	if (sign) *s2++ = '-';
	for (i = nndigit - 1; i > 0 && s1[i] == '0'; i--) nndigit--;
	if (dp > ndigit || dp < -(NDIGINEXP(dp)+1)) {
		/* Use E format, otherwise we need too many '0''s */
		dp--;
		*s2++ = *s1++;
		*s2++ = '.';
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
		return buf;
	}
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
	if (s2[-1]=='.') s2--;
	*s2 = '\0';
	return buf;
}
#endif
