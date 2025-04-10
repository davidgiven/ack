/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Id$ */

/********************************************************/
/*
	Type definitions for C Floating Point Package
	include file for floating point package
*/
/********************************************************/
/*
	THESE STRUCTURES ARE USED TO ADDRESS THE INDIVIDUAL
	PARTS OF THE FLOATING POINT NUMBER REPRESENTATIONS.

	THREE STRUCTURES ARE DEFINED:
		SINGLE:	single precision floating format
		DOUBLE:	double precision floating format
		EXTEND:	double precision extended format
*/
/********************************************************/

#ifndef __FPTYPES
#define __FPTYPES

typedef	struct	{
	unsigned long	h_32;	/* higher 32 bits of 64 */
	unsigned long	l_32;	/* lower  32 bits of 64 */
}	B64;

typedef	unsigned long	SINGLE;

typedef	struct	{
	unsigned long	d[2];
}	DOUBLE;

typedef	struct	{	/* expanded float format	*/
	short	sign;
	short	exp;
	B64	mantissa;
#define m1 mantissa.h_32
#define m2 mantissa.l_32
} EXTEND;

struct	fef4_returns {
	int	e;
	SINGLE	f;
};

struct	fef8_returns {
	int	e;
	DOUBLE	f;
};

struct fif4_returns {
	SINGLE ipart;
	SINGLE fpart;
};

struct fif8_returns {
	DOUBLE ipart;
	DOUBLE fpart;
};

#if __STDC__
#define _PROTOTYPE(function, params)	function params
#else
#define _PROTOTYPE(function, params)	function()
#endif
 void add_ext(EXTEND *e1, EXTEND *e2);
 void mul_ext(EXTEND *e1, EXTEND *e2);
 void div_ext(EXTEND *e1, EXTEND *e2);
 void sub_ext(EXTEND *e1, EXTEND *e2);
 void sft_ext(EXTEND *e1, EXTEND *e2);
 void nrm_ext(EXTEND *e1);
 void zrf_ext(EXTEND *e1);
 void extend(unsigned long *from, EXTEND *to, int size);
 void compact(EXTEND *from, unsigned long *to, int size);
 void _fptrp(int);
 void adf4(SINGLE s2, SINGLE s1);
 void adf8(DOUBLE s2, DOUBLE s1);
 void sbf4(SINGLE s2, SINGLE s1);
 void sbf8(DOUBLE s2, DOUBLE s1);
 void dvf4(SINGLE s2, SINGLE s1);
 void dvf8(DOUBLE s2, DOUBLE s1);
 void mlf4(SINGLE s2, SINGLE s1);
 void mlf8(DOUBLE s2, DOUBLE s1);
 void ngf4(SINGLE f);
 void ngf8(DOUBLE f);
 void zrf4(SINGLE *l);
 void zrf8(DOUBLE *z);
 void cff4(DOUBLE src);
 void cff8(SINGLE src);
 void cif4(int ss, long src);
 void cif8(int ss, long src);
 void cuf4(int ss, long src);
 void cuf8(int ss, long src);
 long cfu(int ds, int ss, DOUBLE src);
 long cfi(int ds, int ss, DOUBLE src);
 int cmf4(SINGLE s2, SINGLE s1);
 int cmf8(DOUBLE d1, DOUBLE d2);
 void fef4(struct fef4_returns *r, SINGLE s1);
 void fef8(struct fef8_returns *r, DOUBLE s1);
 void fif4(struct fif4_returns *p, SINGLE x, SINGLE y);
 void fif8(struct fif8_returns *p, DOUBLE x, DOUBLE y);

 void b64_sft(B64 *, int);
 void b64_lsft(B64 *);
 void b64_rsft(B64 *);
 int b64_add(B64 *, B64 *);
#endif
