/*
 * localmath.h - This header is used by the mathematical library.
 */
/* $Id$ */

/* macros for constructing polynomials */
#define	POLYNOM1(x, a)	((a)[1]*(x)+(a)[0])
#define	POLYNOM2(x, a)	(POLYNOM1((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM3(x, a)	(POLYNOM2((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM4(x, a)	(POLYNOM3((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM5(x, a)	(POLYNOM4((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM6(x, a)	(POLYNOM5((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM7(x, a)	(POLYNOM6((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM8(x, a)	(POLYNOM7((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM9(x, a)	(POLYNOM8((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM10(x, a)	(POLYNOM9((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM11(x, a)	(POLYNOM10((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM12(x, a)	(POLYNOM11((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM13(x, a)	(POLYNOM12((x),(a)+1)*(x)+(a)[0])

/* These are set up for 64-bit doubles. */

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

