/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

#include "flt_misc.h"

void
flt_mul(e1,e2,e3)
	register flt_arith *e1,*e2,*e3;
{
	/*	Multiply the extended numbers e1 and e2, and put the
		result in e3.
	*/
	register int	i,j;		/* loop control	*/
	unsigned short	mp[4];
	unsigned short	mc[4];
	unsigned short	result[8];	/* result */

	register unsigned short *pres;

	flt_status = 0;

	/* first save the sign (XOR)			*/
	e3->flt_sign = e1->flt_sign ^ e2->flt_sign;

	/* compute new exponent */
	e3->flt_exp = e1->flt_exp + e2->flt_exp + 1;

	/* 128 bit multiply of mantissas	*/

	/* assign unknown long formats		*/
	/* to known unsigned word formats	*/
	flt_split(e1, mp);
	flt_split(e2, mc);
	for (i = 8; i--;) {
		result[i] = 0;
	}
	/*
	 *	fill registers with their components
	 */
	for(i=4, pres = &result[4];i--;pres--) if (mp[i]) {
		unsigned short k = 0;
		long mpi = mp[i];
		for(j=4;j--;) {
			long tmp = (long)pres[j] + k;
			if (mc[j]) tmp += mpi * mc[j];
			pres[j] = tmp & 0xFFFF;
			k = (tmp >> 16) & 0xFFFF;
		}
		pres[-1] = k;
	}

	if (! (result[0] & 0x8000)) {
		e3->flt_exp--;
		for (i = 0; i <= 3; i++) {
			result[i] <<= 1;
			if (result[i+1]&0x8000) result[i] |= 1;
		}
		result[4] <<= 1;
	}	
	/*
	 *	combine the registers to a total
	 */
	e3->m1 = ((long)result[0] << 16) + result[1];
	e3->m2 = ((long)result[2] << 16) + result[3];
	if (result[4] & 0x8000) {
		if (++e3->m2 == 0 || (e3->m2 & ~ 0xFFFFFFFF)) {
			e3->m2 = 0;
			if (++e3->m1 == 0 || (e3->m1 & ~ 0xFFFFFFFF)) {
				e3->m1 = 0x80000000;
				e3->flt_exp++;
			}
		}
	}
	flt_chk(e3);
}
