/*
 * (c) copyright 1989 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef __FLT_INCLUDED__
#define __FLT_INCLUDED__

struct _mantissa {
	long	flt_h_32;
	long	flt_l_32;
};

struct _EXTEND {
	short	flt_sign;
	short	flt_exp;
	struct _mantissa flt_mantissa;
};

typedef struct _EXTEND flt_arith;

extern int	flt_status;
#define FLT_OVFL	001
#define FLT_UNFL	002
#define FLT_DIV0	004
#define	FLT_NOFLT	010
#define FLT_BTSM	020

#endif __FLT_INCLUDED__
