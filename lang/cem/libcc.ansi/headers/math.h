/*
 * math.h - mathematics
 */
/* $Header$ */

#ifndef	_MATH_HEADER_
#define	_MATH_HEADER_

#include	<float.h>
#define	HUGE_VAL	DBL_MAX

double	acos(double x);
double	asin(double x);
double	atan(double x);
double	atan2(double y, double x);

double	cos(double x);
double	sin(double x);
double	tan(double x);

double	cosh(double x);
double	sinh(double x);
double	tanh(double x);

double	exp(double x);
double	log(double x);
double	log10(double x);

double	sqrt(double x);
double	ceil(double x);
double	fabs(double x);
double	floor(double x);

double	pow(double x, double y);

double	frexp(double x, int *exp);
double	ldexp(double x, int exp);
double	modf(double x, double *iptr);
double	fmod(double x, double y);

#endif	/* _MATH_HEADER_ */
