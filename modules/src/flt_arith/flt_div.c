/*
  (c) copyright 1989 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Id$ */

#include <stdint.h>
#include "flt_misc.h"

void
flt_div(e1,e2,e3)
	register flt_arith *e1,*e2,*e3;
{
	uint32_t result[2];
	register uint32_t *rp;
	unsigned short u[9], v[5];
	register int j;
	register unsigned short *u_p = u;
	int maxv = 4;
	flt_arith cpe1 = *e1;

	flt_status = 0;
	e3->flt_sign = e1->flt_sign ^ e2->flt_sign;
	if ((e2->m1 | e2->m2) == 0) {
		flt_status = FLT_DIV0;
		e3->flt_exp = EXT_MAX;
		e3->m1 = 0x80000000;
		e3->m2 = 0L;
		return;
	}
	if ((e1->m1 | e1->m2) == 0) {	/* 0 / anything == 0 */
		e3->flt_exp = 0;	/* make sure */
		e3->m1 = e3->m2 = 0L;
		e3->flt_sign = 0;
		return;
	}
	e3->flt_exp = e1->flt_exp - e2->flt_exp;

	u[4] = (e1->m2 & 1) << 15;
	flt_b64_sft(&(cpe1.flt_mantissa),1);
	flt_split(&cpe1, u);
	u[5] = 0; u[6] = 0; u[7] = 0;
	flt_split(e2, &v[1]);
	while (! v[maxv]) maxv--;
	result[0] = 0;
	result[1] = 0;
	rp = result;

	/*
	 * Use an algorithm of Knuth (The art of programming, Seminumerical
	 * algorithms), to divide u by v. u and v are both seen as numbers
	 * with base 65536. 
	 */
	for (j = 0; j <= 3; j++, u_p++) {
		uint32_t q_est, temp;

		if (j == 2) rp++;
		if (u_p[0] == 0 && u_p[1] < v[1]) continue;
		temp = ((uint32_t)u_p[0] << 16) + u_p[1];
		if (u_p[0] >= v[1]) {
			q_est = 0x0000FFFFUL;
		}
		else if (v[1] == 1) {
			q_est = temp;
		}
		else {
			q_est = temp / v[1];
		}
		temp -= q_est * v[1];
		while (!(temp&0xFFFF0000) &&
		       v[2]*q_est > (temp<<16)+u_p[2]) {
			q_est--;
			temp += v[1];
		}
		/*	Now, according to Knuth, we have an estimate of the
			quotient, that is either correct or one too big, but
			almost always correct.
		*/
		if (q_est != 0)  {
			int i;
			uint32_t k = 0;
			int borrow = 0;

			for (i = maxv; i > 0; i--) {
				uint32_t tmp = q_est * v[i] + k + borrow;
				unsigned short md = tmp & 0xFFFF;

				borrow = (md > u_p[i]);
				u_p[i] -= md;
				k = (tmp >> 16) & 0xFFFF;
			}
			k += borrow;
			borrow = u_p[0] < k;
			u_p[0] -= k;

			if (borrow) {
				/* So, this does not happen often; the estimate
				   was one too big; correct this
				*/
				q_est--;
				borrow = 0;
				for (i = maxv; i > 0; i--) {
					uint32_t tmp
					    = v[i]+(uint32_t)u_p[i]+borrow;

					u_p[i] = tmp & 0xFFFF;
					borrow = (tmp >> 16) & 0xFFFF;
				}
				u_p[0] += borrow;
			}
			*rp |= (j & 1) ? q_est : (q_est<<16);
		}
	}
	e3->m1 = result[0];
	e3->m2 = result[1];

	flt_nrm(e3);
	flt_chk(e3);
}
