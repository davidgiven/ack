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

#define	M_LN_MAX_D	(M_LN2 * DBL_MAX_EXP)
#define	M_LN_MIN_D	(M_LN2 * (DBL_MIN_EXP - 1))
