/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* $Id$ */

#ifndef _MATH_H
#define _MATH_H

/* some constants (Hart & Cheney) */

#define M_PI		 3.14159265358979323846264338327950288
#define M_2PI		 6.28318530717958647692528676655900576
#define M_3PI_4		 2.35619449019234492884698253745962716
#define M_PI_2		 1.57079632679489661923132169163975144
#define M_3PI_8		 1.17809724509617246442349126872981358
#define M_PI_4		 0.78539816339744830961566084581987572
#define M_PI_8		 0.39269908169872415480783042290993786
#define M_1_PI		 0.31830988618379067153776752674502872
#define M_2_PI		 0.63661977236758134307553505349005744
#define M_4_PI		 1.27323954473516268615107010698011488
#define M_E		 2.71828182845904523536028747135266250
#define M_LOG2E		 1.44269504088896340735992468100189213
#define M_LOG10E	 0.43429448190325182765112891891660508
#define M_LN2		 0.69314718055994530941723212145817657
#define M_LN10		 2.30258509299404568401799145468436421
#define M_SQRT2		 1.41421356237309504880168872420969808
#define M_1_SQRT2	 0.70710678118654752440084436210484904
#define M_EULER		 0.57721566490153286060651209008240243

#ifndef __NO_DEFS
extern double exp(), log(), log10(), pow(), sqrt();
extern double fabs(), ceil(), floor();
extern double gamma();
extern double hypot(), cabs();
extern double j0(), y0(), j1(), y1(), jn(), yn();
extern double sin(), asin(), tan(), atan(), atan2(), cos(), acos();
extern double sinh(), tanh(), cosh();
#endif

#define POLYNOM1(x, a)	((a)[1]*(x)+(a)[0])
#define POLYNOM2(x, a)	(POLYNOM1((x),(a)+1)*(x)+(a)[0])
#define POLYNOM3(x, a)	(POLYNOM2((x),(a)+1)*(x)+(a)[0])
#define POLYNOM4(x, a)	(POLYNOM3((x),(a)+1)*(x)+(a)[0])
#define POLYNOM5(x, a)	(POLYNOM4((x),(a)+1)*(x)+(a)[0])
#define POLYNOM6(x, a)	(POLYNOM5((x),(a)+1)*(x)+(a)[0])
#define POLYNOM7(x, a)	(POLYNOM6((x),(a)+1)*(x)+(a)[0])
#define POLYNOM8(x, a)	(POLYNOM7((x),(a)+1)*(x)+(a)[0])
#define POLYNOM9(x, a)	(POLYNOM8((x),(a)+1)*(x)+(a)[0])
#define POLYNOM10(x, a)	(POLYNOM9((x),(a)+1)*(x)+(a)[0])
#define POLYNOM11(x, a)	(POLYNOM10((x),(a)+1)*(x)+(a)[0])
#define POLYNOM12(x, a)	(POLYNOM11((x),(a)+1)*(x)+(a)[0])
#define POLYNOM13(x, a)	(POLYNOM12((x),(a)+1)*(x)+(a)[0])

#ifdef __vax
#define M_MAX_D		 1.701411834604692293e+38
#define M_MIN_D		 2.938735877055718770e-39
#define M_DEXPLEN	 8
#endif

#ifdef __pdp
#define M_MAX_D		 1.701411834604692293e+38
#define M_MIN_D		 2.938735877055718770e-39
#define M_DEXPLEN	 8
#endif

#ifndef M_MAX_D
#define M_MAX_D          1.7976931348623157e+308
#define M_MIN_D		 2.2250738585072014e-308
#define M_DEXPLEN	 11
#endif
#define M_DMAXEXP	 ((1 << (M_DEXPLEN - 1)) - 1)
#define M_DMINEXP	 (-M_DMAXEXP)
#define M_LN_MAX_D	 (M_LN2 * M_DMAXEXP)
#define M_LN_MIN_D	 (M_LN2 * (M_DMINEXP - 1))

#define HUGE		M_MAX_D
#define MAXDOUBLE	M_MAX_D

#endif /* _MATH_H */
