/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	ROUTINE TO MULTIPLY TWO EXTENDED FORMAT NUMBERS
*/

# include "adder.h"
# include "FP_bias.h"
# include "FP_trap.h"
# include "FP_types.h"

mul_ext(e1,e2)
EXTEND	*e1,*e2;
{
	register int	i,j;		/* loop control	*/
	short unsigned	mp[4];	/* multiplier */
	short unsigned	mc[4];	/* multipcand */
	short unsigned	result[8];	/* result */
	B64		tmp64;
	register unsigned short *pres;

	/* first save the sign (XOR)			*/

	e1->sign ^= e2->sign;

	/********************************************************/
	/* 	 	INCREASE EXPONENT BY ONE (1)	    	*/
	/*							*/
	/* the nature of the multiplication algorithm used	*/
	/* results in an exponent that is small by an additive	*/
	/* factor of one (1);					*/
	/* if the maximum bit is set it will not be subtracted	*/
	/* during normalization -> this is correct and can be	*/
	/* expected often with normalized numbers		*/
	/*	HOWEVER, it is also possible that unnormalized	*/
	/*	numbers are used. Rather than shifting here	*/
	/*	always(!) (unless L bit is set) I chose to	*/
	/*	increase the exponent by one - a simple (FAST)	*/
	/*	process - and to decrease it later during	*/
	/*	normalization.					*/
	/*							*/
	/********************************************************/
	/* The effects of bias (as used here)			*/
	/* and the multiplication algorithm used cancel		*/
	/* so these statements are commented out		*/
	/* August 1985 - if changing the Leading Bit (or NORMBIT) */
	/* this problem with the multiplication algorithm no longer */
	/* exists - bias must be subtracted now			*/
	/*							*/
	/* e1->exp++;						*/
	/********************************************************/

	/* next add the exponents			*/

	e1->exp += e2->exp;
	e1->exp -= 1;			/* correction for bias	*/

					/* check for overflow	*/
	if (e1->exp >= EXT_MAX)	{
		trap(EFOVFL);
			/* if caught 			*/
			/* return signed infinity	*/
		e1->exp = EXT_MAX;
infinity:	e1->m1 = e1->m2 =0L;
		return;
	}
				/* check for underflow	*/
	if (e1->exp < EXT_MIN)	{
		trap(EFUNFL);
		e1->exp = EXT_MIN;
		goto infinity;
	}

	/* 128 bit multiply of mantissas			*/

		/* assign unknown long formats		*/
		/* to known unsigned word formats	*/
	mp[0] = e1->m1 >> 16;
	mp[1] = (unsigned short) e1->m1;
	mp[2] = e1->m2 >> 16;
	mp[3] = (unsigned short) e1->m2;
	mc[0] = e2->m1 >> 16;
	mc[1] = (unsigned short) e2->m1;
	mc[2] = e2->m2 >> 16;
	mc[3] = (unsigned short) e2->m2;
	for (i = 8; i--;) {
		result[i] = 0;
	}
	/*
	 *	fill registers with their components
	 */
	for(i=4, pres = &result[4];i--;pres--) if (mp[i]) {
		unsigned short k = 0;
		unsigned long mpi = mp[i];
		for(j=4;j--;) {
			unsigned long tmp = (unsigned long)pres[j] + k;
			if (mc[j]) tmp += mpi * mc[j];
			pres[j] = tmp;
			k = tmp >> 16;
		}
		pres[-1] = k;
	}
	
	/*
	 *	combine the registers to a total
	 */
	e1->m1 = ((unsigned long)(result[0]) << 16) + result[1];
	e1->m2 = ((unsigned long)(result[2]) << 16) + result[3];
	if (result[4] & 0x8000) {
		if (++e1->m2 == 0)
			e1->m1++;
	}

	nrm_ext(e1);
}
