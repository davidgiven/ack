/*
 * (c) copyright 1989 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include <flt_arith.h>

/* some short-hands ... */
#define m1 flt_mantissa.flt_h_32
#define m2 flt_mantissa.flt_l_32

/* some constants */
#define EXT_MAX		16384		/* max exponent */
#define EXT_MIN		(-16384)	/* min exponent */

/* hiding of names: */
#define ucmp		_flt_ucmp
#define flt_nrm		_flt_nrm
#define flt_chk		_flt_chk
#define flt_b64_add	_flt_64add
#define flt_split	_flt_split

_PROTOTYPE(int ucmp, (long, long));
_PROTOTYPE(void flt_nrm, (flt_arith *));
_PROTOTYPE(void flt_chk, (flt_arith *));
_PROTOTYPE(int flt_b64_add, (struct flt_mantissa *, struct flt_mantissa *));
_PROTOTYPE(void flt_split, (flt_arith *, unsigned short *));
