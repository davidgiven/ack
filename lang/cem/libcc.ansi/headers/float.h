/*
 * float.h - implementation limits
 */
/* $Header$ */

#ifndef	_FLOAT_HEADER_
#define	_FLOAT_HEADER_

#ifdef	vax
#define	FLT_DIG			6
#define	FLT_EPSILON		5.96046448e-08
#define	FLT_MANT_DIG		8
#define	FLT_MAX			1.70141173e+38
#define	FLT_MAX_10_EXP		38
#define	FLT_MAX_EXP		127
#define	FLT_MIN			2.93873588e-39
#define	FLT_MIN_10_EXP		-39
#define	FLT_MIN_EXP		-127

#define	DBL_DIG			16
#define	DBL_EPSILON		1.38777878078144568e-17
#define	DBL_MANT_DIG		8
#define	DBL_MAX			1.70141183460469229e+38
#define	DBL_MAX_10_EXP		38
#define	DBL_MAX_EXP		127
#define	DBL_MIN			2.93873587705571877e-39
#define	DBL_MIN_10_EXP		39
#define	DBL_MIN_EXP		-127

#define	LDBL_DIG		16
#define	LDBL_EPSILON		1.38777878078144568e-17
#define	LDBL_MANT_DIG		8
#define	LDBL_MAX		1.70141183460469229e+38
#define	LDBL_MAX_10_EXP		38
#define	LDBL_MAX_EXP		127
#define	LDBL_MIN		2.93873587705571877e-39
#define	LDBL_MIN_10_EXP		39
#define	LDBL_MIN_EXP		-127

#define	FLT_ROUNDS		1
#define	FLT_RADIX		2

#else
#ifdef	pdp
/* The values are not certain, because there are no pdp's here anymore. The
 * values given here are the same as for the vax. 
 */
#define	FLT_DIG			6
#define	FLT_EPSILON		5.96046448e-08
#define	FLT_MANT_DIG		8
#define	FLT_MAX			1.70141173e+38
#define	FLT_MAX_10_EXP		38
#define	FLT_MAX_EXP		127
#define	FLT_MIN			2.93873588e-39
#define	FLT_MIN_10_EXP		-39
#define	FLT_MIN_EXP		-127

#define	DBL_DIG			16
#define	DBL_EPSILON		1.38777878078144568e-17
#define	DBL_MANT_DIG		8
#define	DBL_MAX			1.70141183460469229e+38
#define	DBL_MAX_10_EXP		38
#define	DBL_MAX_EXP		127
#define	DBL_MIN			2.93873587705571877e-39
#define	DBL_MIN_10_EXP		39
#define	DBL_MIN_EXP		-127

#define	LDBL_DIG		16
#define	LDBL_EPSILON		1.38777878078144568e-17
#define	LDBL_MANT_DIG		8
#define	LDBL_MAX		1.70141183460469229e+38
#define	LDBL_MAX_10_EXP		38
#define	LDBL_MAX_EXP		127
#define	LDBL_MIN		2.93873587705571877e-39
#define	LDBL_MIN_10_EXP		39
#define	LDBL_MIN_EXP		-127

#define	FLT_ROUNDS		1
#define	FLT_RADIX		2

#else		/* floating point emulation */
#define	FLT_DIG			6
#define	FLT_EPSILON		5.96046448e-08
#define	FLT_MANT_DIG		8
#define	FLT_MAX			1.70141173e+38
#define	FLT_MAX_10_EXP		38
#define	FLT_MAX_EXP		127
#define	FLT_MIN			2.93873588e-39
#define	FLT_MIN_10_EXP		-39
#define	FLT_MIN_EXP		-127

#define	DBL_DIG			15
#define	DBL_EPSILON		1.1102230246251565e-16
#define	DBL_MANT_DIG		11
#define	DBL_MAX			8.9884656743115823e+307
#define	DBL_MAX_10_EXP		307
#define	DBL_MAX_EXP		1023
#define	DBL_MIN			5.5626846462680062e-309
#define	DBL_MIN_10_EXP		-309
#define	DBL_MIN_EXP		-1023

#define	LDBL_DIG		15
#define	LDBL_EPSILON		1.1102230246251565e-16
#define	LDBL_MANT_DIG		11
#define	LDBL_MAX		8.9884656743115823e+307
#define	LDBL_MAX_10_EXP		307
#define	LDBL_MAX_EXP		1023
#define	LDBL_MIN		5.5626846462680062e-309
#define	LDBL_MIN_10_EXP		-309
#define	LDBL_MIN_EXP		-1023

#define	FLT_ROUNDS		1
#define	FLT_RADIX		2

#endif	/* pdp */
#endif	/* vax */

#endif	/* _FLOAT_HEADER_ */
