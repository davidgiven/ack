/*
 * math.h - mathematics
 */
/* $Id$ */

#ifndef _MATH_H
#define	_MATH_H

extern double __huge_val;
#define	HUGE_VAL (__huge_val)

extern int	__IsNan(double d);	/* test for Not A Number */

extern double acos(double _x);
extern double asin(double _x);
extern double atan(double _x);
extern double atan2(double _y, double _x);

extern double cos(double _x);
extern double sin(double _x);
extern double tan(double _x);

extern double cosh(double _x);
extern double sinh(double _x);
extern double tanh(double _x);

extern double exp(double _x);
extern double log(double _x);
extern double log10(double _x);

extern double sqrt(double _x);
extern double ceil(double _x);
extern double fabs(double _x);
extern double floor(double _x);

extern double hypot(double _x, double _y);
extern double pow(double _x, double _y);

extern double frexp(double _x, int *_exp);
extern double ldexp(double _x, int _exp);
extern double modf(double _x, double *_iptr);
extern double fmod(double _x, double _y);

/* Some constants (Hart & Cheney). */

#define	M_PI		3.14159265358979323846264338327950288
#define	M_2PI		6.28318530717958647692528676655900576
#define	M_3PI_4		2.35619449019234492884698253745962716
#define	M_PI_2		1.57079632679489661923132169163975144
#define	M_3PI_8		1.17809724509617246442349126872981358
#define	M_PI_4		0.78539816339744830961566084581987572
#define	M_PI_8		0.39269908169872415480783042290993786
#define	M_1_PI		0.31830988618379067153776752674502872
#define	M_2_PI		0.63661977236758134307553505349005744
#define	M_4_PI		1.27323954473516268615107010698011488
#define	M_E		2.71828182845904523536028747135266250
#define	M_LOG2E		1.44269504088896340735992468100189213
#define	M_LOG10E	0.43429448190325182765112891891660508
#define	M_LN2		0.69314718055994530941723212145817657
#define	M_LN10		2.30258509299404568401799145468436421
#define	M_SQRT2		1.41421356237309504880168872420969808
#define	M_1_SQRT2	0.70710678118654752440084436210484904
#define	M_EULER		0.57721566490153286060651209008240243

#endif	/* _MATH_H */
