/*
 * floatpr.c - print floating point numbers
 */
/* $Header$ */

#ifndef	NOFLOAT
#include	"loc_incl.h"

char *
_pfloat(double r, register char *s, int n, int flags)
{
	register char *s1;
	int sign, dp;
	register int i;

	s1 = fcvt(r, n, &dp, &sign);
	if (sign)
		*s++ = '-';
	else if (flags & FL_SIGN)
		*s++ = '+';
	else if (flags & FL_SPACE)
		*s++ = ' ';

	if (dp<=0)
		*s++ = '0';
	for (i=dp; i>0; i--)
		if (*s1) *s++ = *s1++;
		else *s++ = '0';
	if (((i=n) > 0) || (flags & FL_ALT))
		*s++ = '.';
	while (++dp <= 0) {
		if (--i<0)
			break;
		*s++ = '0';
	}
	while (--i >= 0)
		if (*s1) *s++ = *s1++;
		else *s++ = '0';
	return(s);
}

char *
_pscien(double r, register char *s, int n, int flags)
{
	int sign, dp; 
	register char *s1;

	s1 = ecvt(r, n + 1, &dp, &sign);
	if (sign)
		*s++ = '-';
	else if (flags & FL_SIGN)
		*s++ = '+';
	else if (flags & FL_SPACE)
		*s++ = ' ';

	*s++ = *s1++;
	if ((n > 0) || (flags & FL_ALT))
		*s++ = '.';
	while (--n>0)
		if (*s1) *s++ = *s1++;
		else *s++ = '0';
	*s++ = 'e';
	if ( r != 0 ) --dp ;
	if ( dp<0 ) {
		*s++ = '-' ; dp= -dp ;
	} else {
		*s++ = '+' ;
	}
	if (dp >= 100) {
		*s++ = '0' + (dp / 100);
		dp %= 100;
	}
	*s++ = '0' + (dp/10);
	*s++ = '0' + (dp%10);
	return(s);
}

#if	EM_DSIZE != EM_LDSIZE
char *
_pfloat_ldbl(long double r, register char *s, int n, int flags)
{
	register char *s1;
	int sign, dp;
	register int i;

	s1 = fcvt(r, n, &dp, &sign);
	if (sign)
		*s++ = '-';
	else if (flags & FL_SIGN)
		*s++ = '+';
	else if (flags & FL_SPACE)
		*s++ = ' ';

	if (dp<=0)
		*s++ = '0';
	for (i=dp; i>0; i--)
		if (*s1) *s++ = *s1++;
		else *s++ = '0';
	if (((i=n) > 0) || (flags & FL_ALT))
		*s++ = '.';
	while (++dp <= 0) {
		if (--i<0)
			break;
		*s++ = '0';
	}
	while (--i >= 0)
		if (*s1) *s++ = *s1++;
		else *s++ = '0';
	return(s);
}

char *
_pscien_ldbl(long double r, register char *s, int n, int flags)
{
	int sign, dp; 
	register char *s1;

	s1 = ecvt(r, n + 1, &dp, &sign);
	if (sign)
		*s++ = '-';
	else if (flags & FL_SIGN)
		*s++ = '+';
	else if (flags & FL_SPACE)
		*s++ = ' ';

	*s++ = *s1++;
	if ((n > 0) || (flags & FL_ALT))
		*s++ = '.';
	while (--n>0)
		if (*s1) *s++ = *s1++;
		else *s++ = '0';
	*s++ = 'e';
	if ( r != 0 ) --dp ;
	if ( dp<0 ) {
		*s++ = '-' ; dp= -dp ;
	} else {
		*s++ = '+' ;
	}
	if (dp >= 100) {
		*s++ = '0' + (dp / 100);
		dp %= 100;
	}
	*s++ = '0' + (dp/10);
	*s++ = '0' + (dp%10);
	return(s);
}
#endif	/* EM_DSIZE != EM_LDSIZE */
#endif	/* NOFLOAT */
