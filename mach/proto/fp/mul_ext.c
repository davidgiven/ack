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
	register int	k,i,j;		/* loop control	*/
	long  unsigned	*reg[7];
	long  unsigned	tmp[4];
	short unsigned	mp[4];	/* multiplier */
	short unsigned	mc[4];	/* multipcand */
	B64	low64,tmp64;	/* 64 bit storage	*/

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
	/*
	 *	assign pointers
	 */
	reg[0] = &e1->m1;	/* the answer goes here */
	reg[1] = &tmp[1];
	reg[2] = &e1->m2;	/* and here	*/
	reg[3] = &tmp[2];
	reg[4] = &low64.h_32;
	reg[5] = &tmp[3];
	reg[6] = &low64.l_32;

	/*
	 *	zero registers
	 */
	for(i=7;i--;)
		*reg[i] = 0;

	/*
	 *	fill registers with their components
	 */
	for(i=4;i--;) if (mp[i])
		for(j=4;j--;) if (mc[j]) {
			k = i+j;
			tmp[0] = (long)mp[i] * (long)mc[j];
			if (b32_add(reg[k],tmp))	{
				for(tmp[0] = 0x10000L;k>0;)
					if (b32_add(reg[--k],tmp) == 0)
						break;
			}
		}
	
	/*
	 *	combine the registers to a total
	 */
	tmp64.h_32 = (*reg[1]>>16);
	tmp64.l_32 = (*reg[1]<<16) + (*reg[3]>>16);
	b64_add((B64 *)&e1->m1,&tmp64);
	tmp64.l_32 = *reg[5]<<16;
	tmp64.h_32 = (*reg[5]>>16) + (*reg[3]<<16);
	if (b64_add(&low64,&tmp64))
		if (++e1->m2 == 0)
			e1->m1++;

	nrm_ext(e1);
}
