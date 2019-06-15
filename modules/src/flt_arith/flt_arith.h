/*
 * (c) copyright 1989 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/** @file
 *  High precision floating point arithmetic module.
 *
 *  Before each operation, the `#flt_status` variable is reset to 0.
 *
 *
 */
#ifndef __FLT_INCLUDED__
#define __FLT_INCLUDED__

#include <stdint.h>

#ifndef arith
#define arith long
#endif

struct flt_mantissa {
	uint32_t flt_h_32;	/**< high order 32 bits of mantissa */
	uint32_t flt_l_32;	/**< low order 32 bits of mantissa */
};

typedef struct flt_arith {
	short	flt_sign;	/**< 0 for positive, 1 for negative */
	short	flt_exp;	/**< between -16384 and 16384 */
	struct flt_mantissa flt_mantissa; /**< mantissa normalized, in [1,2). */
} flt_arith;

/** Status of last floating point operation. */
extern int	flt_status;
/** An overflow occurred. */
#define FLT_OVFL	001
/** An underflow occurred. The result is 0. */
#define FLT_UNFL	002
/** Divide by 0. */
#define FLT_DIV0	004
/** String cannot be represented into a real number. */
#define	FLT_NOFLT	010
/** Buffer is too small. */
#define FLT_BTSM	020

#define FLT_STRLEN	32	/**< max length of result of flt_flt2str() */

/** Adds the numbers indicated by `e1` and `e2` and stores the result
 *  in `res`. */
void flt_add(flt_arith *e1, flt_arith *e2, flt_arith *res);
/** Subtracts the number indicated by `e2` from the one indicated by
 *   `e1` and stores the result in `res`. */
void flt_sub(flt_arith *e1, flt_arith *e2, flt_arith *res);
/** Multiplies the numbers indicated by `e1` and `e2` and stores the
 *  result in `res`. */
void flt_mul(flt_arith *e1, flt_arith *e2, flt_arith *res);
/** Divides the number indicated by `e1` by the one indicated by `e2`
 *  and stores the result in `res`.
 */
void flt_div(flt_arith *e1, flt_arith *e2, flt_arith *res);
/** Splits the number indicated by `e` in an integer and a fraction
 *  part, and stores the integer part through `ipart` and the fraction part
 *  through `fpart`.  Also, the absolute value of the number indicated by
 *  `ipart` is less than or equal to the absolute value of the number
 *  indicated by `e`. The absolute value of the number indicated by
 *  `fpart` is less than 1.
 */
void flt_modf(flt_arith *e, flt_arith *ipart, flt_arith *fpart);
/** Compares  the numbers indicated by `e1` and `e2` and returns -1 if
 *  `e1` < `e2`, 0 if `e1` = `e2`, and 1 if `e1` > `e2`.
 */
int flt_cmp(flt_arith *e1, flt_arith *e2);

/** Converts the string indicated by `s` to a floating point number,
 *  and stores this  number through `e`.  The string should contain a
 *  floating point constant, which consists of an integer part, a decimal
 *  point, a fraction part, an e or an E, and an optionally signed integer
 *  exponent. The integer and fraction parts both consist of a sequence of
 *  digits.  They may not both be missing. The decimal point, the e and the
 *  exponent may be missing.
 */
void flt_str2flt(char *s, flt_arith *e);
/** Converts the number indicated by `e` into a string, in a scientific
 *  notation acceptable for EM. The result is stored in `buf`. At most
 *  `bufsize` characters are  stored.   The  maximum  length  needed  is
 *  available in the constant `#FLT_STRLEN`.
 */
void flt_flt2str(flt_arith *e, char *buf, int bufsize);
/** Converts the number `n` to the floating point format used
 *  in this package and returns the result in `e`. If the `uns`
 *  flag is set, the number n is regarded as an unsigned.
 */
void flt_arith2flt(arith n, flt_arith *e, int uns);
/** Truncates the number indicated by `e` to the largest integer
 *  value smaller than or equal to the number indicated by `e`. It
 *  returns this value. If the uns flag is set, the result is to be
 *  regarded as unsigned.
 */
arith flt_flt2arith(register flt_arith *e, int uns);
/** Negates the number indicated by `e` and stores the result
 *  in `e`. */
void flt_umin(flt_arith *e);
/** Shifts the mantissa m |n| bits left or right, depending on the sign
 *  of n. If n is negative, it is a left-shift; If n is positive,
 *  it is a right shift.
 */
void flt_b64_sft(
	register struct flt_mantissa *e,
	register int n);




#endif /* __FLT_INCLUDED__ */
