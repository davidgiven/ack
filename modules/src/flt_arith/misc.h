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
#define ucmp		flt__ucmp
#define flt_nrm		flt__nrm
#define flt_chk		flt__chk
#define flt_b64_add	flt__64add
#define flt_split	flt__split
