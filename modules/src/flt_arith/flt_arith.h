/*
 * (c) copyright 1989 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef __FLT_INCLUDED__
#define __FLT_INCLUDED__

struct flt_mantissa {
	long	flt_h_32;	/* high order 32 bits of mantissa */
	long	flt_l_32;	/* low order 32 bits of mantissa */
};

typedef struct {
	short	flt_sign;	/* 0 for positive, 1 for negative */
	short	flt_exp;	/* between -16384 and 16384 */
	struct flt_mantissa flt_mantissa;	/* normalized, in [1,2). */
} flt_arith;

extern int	flt_status;
#define FLT_OVFL	001
#define FLT_UNFL	002
#define FLT_DIV0	004
#define	FLT_NOFLT	010
#define FLT_BTSM	020

#define FLT_STRLEN	32	/* max length of result of flt_flt2str() */
#endif /* __FLT_INCLUDED__ */
