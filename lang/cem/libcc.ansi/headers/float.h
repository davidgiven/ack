/*
 * float.h - implementation limits
 */
/* $Id$ */

#if	!defined(_FLOAT_H)
#define	_FLOAT_H

#if	defined(__vax) || defined(__pdp)
#define	FLT_DIG			6
#define	FLT_EPSILON		5.96046448e-08F
#define	FLT_MANT_DIG		8
#define	FLT_MAX			1.70141173e+38F
#define	FLT_MAX_10_EXP		38
#define	FLT_MAX_EXP		127
#define	FLT_MIN			2.93873588e-39F
#define	FLT_MIN_10_EXP		(-38)
#define	FLT_MIN_EXP		(-127)

#define	DBL_DIG			16
#define	DBL_EPSILON		1.38777878078144568e-17
#define	DBL_MANT_DIG		8
#define	DBL_MAX			1.70141183460469229e+38
#define	DBL_MAX_10_EXP		38
#define	DBL_MAX_EXP		127
#define	DBL_MIN			2.93873587705571877e-39
#define	DBL_MIN_10_EXP		(-38)
#define	DBL_MIN_EXP		(-127)

#define	LDBL_DIG		16
#define	LDBL_EPSILON		1.38777878078144568e-17L
#define	LDBL_MANT_DIG		8
#define	LDBL_MAX		1.70141183460469229e+38L
#define	LDBL_MAX_10_EXP		38
#define	LDBL_MAX_EXP		127
#define	LDBL_MIN		2.93873587705571877e-39L
#define	LDBL_MIN_10_EXP		(-38)
#define	LDBL_MIN_EXP		(-127)

#define	FLT_ROUNDS		1
#define	FLT_RADIX		2

#else		/* IEEE format */
#define	FLT_DIG			6
#define	FLT_EPSILON		1.19209290e-07F
#define	FLT_MANT_DIG		24
#define	FLT_MAX			3.40282347e+38F
#define	FLT_MAX_10_EXP		38
#define	FLT_MAX_EXP		128
#define	FLT_MIN			1.17549435e-38F
#define	FLT_MIN_10_EXP		(-37)
#define	FLT_MIN_EXP		(-125)

#define	DBL_DIG			15
#define	DBL_EPSILON		2.2204460492503131e-16
#define	DBL_MANT_DIG		53
#define	DBL_MAX			1.7976931348623157e+308
#define	DBL_MAX_10_EXP		308
#define	DBL_MAX_EXP		1024
#define	DBL_MIN			2.2250738585072014e-308
#define	DBL_MIN_10_EXP		(-307)
#define	DBL_MIN_EXP		(-1021)

#define	LDBL_DIG		15
#define	LDBL_EPSILON		2.2204460492503131e-16L
#define	LDBL_MANT_DIG		53
#define	LDBL_MAX		1.7976931348623157e+308L
#define	LDBL_MAX_10_EXP		308
#define	LDBL_MAX_EXP		1024
#define	LDBL_MIN		2.2250738585072014e-308L
#define	LDBL_MIN_10_EXP		(-307)
#define	LDBL_MIN_EXP		(-1021)

#define	FLT_ROUNDS		1
#define	FLT_RADIX		2

#endif	/* vax, pdp or ieee */

#endif	/* _FLOAT_H */
