/*	$Id: softfloat.h,v 1.3 2016/03/05 15:31:25 ragge Exp $	*/

/*
 * Copyright (c) 2015 Anders Magnusson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Definitions for softfloat routines.
 */


typedef struct softfloat {
	unsigned long long significand;
	short exponent;
	short kind;
} SF;
typedef SF *SFP;

#define C(x,y) C2(x,y)
#define C2(x,y) x##y

#ifdef USE_IEEEFP_32
#define	IEEEFP_32_RADIX 2
#define IEEEFP_32_DIG 6
#define IEEEFP_32_EPSILON 1.19209290E-07F
#define IEEEFP_32_MAX_10_EXP +38
#define IEEEFP_32_MAX_EXP +128
#define IEEEFP_32_MAX 3.40282347E+38F
#define IEEEFP_32_MIN_10_EXP -37
#define IEEEFP_32_MIN_EXP -125
#define IEEEFP_32_MIN 1.17549435E-38F
#define IEEEFP_32_MANT_DIG 24
#define IEEEFP_32_DECIMAL_DIG 9

#define IEEEFP_32_TRUE_MIN 1.40129846E-45F
#define IEEEFP_32_HAS_SUBNORM 1
#endif
#ifdef IEEEFP_64
#endif
#ifdef IEEEFP_X80
#endif
#ifdef IEEEFP_128
#endif

#define	TARGET_FLT_RADIX	C(FLT_FP,_RADIX)

#ifndef NATIVE_FLOATING_POINT

/*
 * Description of a floating point format, based what is in gdtoa package.
 * The first members are the same as in gdtoa,  the rest are pcc specific.
 */
typedef struct FPI {
	int nbits;
	int emin;
	int emax;
	int rounding;

	int sudden_underflow:1;
	int explicit_one:1; /* if MSB is explicitely stored */
	int has_inf_nan:1;  /* highest exponent means INF and NaN */
	int has_neg_zero:1;
	int has_radix_16:1;
	int storage;
	int exp_bias;
} FPI;

/* SF.kind values; same as STRTODG_* values */
enum {
	SF_Zero		= 0,
	SF_Normal	= 1,
	SF_Denormal	= 2,
	SF_Infinite	= 3,
	SF_NaN		= 4, /* default quiet NaN */
	SF_NaNbits	= 5, /* (not used) */
	SF_NoNumber	= 6, /* signaling NaN */
	SF_kmask	= 7,

	/* The following may be or-ed into one of the above values. */
	SF_Neg		= 0x80, /* does not affect SFEXCP_Inex(lo|hi) */
	SFEXCP_Inexlo	= 0x100, /* returned result rounded toward zero */
	SFEXCP_Inexhi	= 0x200, /* returned result rounded away from zero */
	SFEXCP_Inexact	= 0x300,
	SFEXCP_Underflow= 0x400,
	SFEXCP_Overflow = 0x800,
	SFEXCP_DivByZero= 0x1000,
	SFEXCP_Invalid	= 0x2000,

	SFEXCP_Aborted	= 0x8000, /* Not IEEE; operation not performed */
	SFEXCP_ALLmask	= 0xFF00 /* All exceptions (mask) */
};

/* FPI.rounding values: same as FLT_ROUNDS */
enum {
	FPI_Round_zero = 0,	/* same meaning as FE_TOWARDZERO */
	FPI_Round_near = 1,	/* same meaning as FE_TONEAREST */
	FPI_Round_up = 2,	/* same meaning as FE_UPWARD */
	FPI_Round_down = 3,	/* same meaning as FE_DOWNWARD */
/* Warning: if adding new modes, keep same meaning for 2 low bits. */
	FPI_Round_near_from0 = 5, /* to nearest but ties up (Vax) */

	FPI_RoundNotSet = -4,	/* to implement dynamic rounding */
};

extern FPI * fpis[3]; /* FLOAT, DOUBLE, LDOUBLE, respectively */

SF soft_neg(SF);
SF soft_int2fp(CONSZ p, TWORD v);
CONSZ soft_fp2int(SF p, TWORD v);
SF soft_fp2fp(SF p, TWORD v);
SFP soft_cast(CONSZ v, TWORD);
SF soft_plus(SF, SF);
SF soft_minus(SF, SF);
SF soft_mul(SF, SF);
SF soft_div(SF, SF);
int soft_cmp(SF, SF, int);
int soft_isz(SF);
SF strtosf(char *);
#endif
