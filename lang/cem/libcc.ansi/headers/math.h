/*
 * math.h - mathematics
 */
/* $Header$ */

#if	!defined(_MATH_H)
#define	_MATH_H

#define	HUGE_VAL	9.9e+999	/* though it will generate a warning */

double	acos(double __x);
double	asin(double __x);
double	atan(double __x);
double	atan2(double __y, double __x);

double	cos(double __x);
double	sin(double __x);
double	tan(double __x);

double	cosh(double __x);
double	sinh(double __x);
double	tanh(double __x);

double	exp(double __x);
double	log(double __x);
double	log10(double __x);

double	sqrt(double __x);
double	ceil(double __x);
double	fabs(double __x);
double	floor(double __x);

double	pow(double __x, double __y);

double	frexp(double __x, int *__exp);
double	ldexp(double __x, int __exp);
double	modf(double __x, double *__iptr);
double	fmod(double __x, double __y);

#endif	/* _MATH_H */
