/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
#define	PRT_EXT
#define	PRT_ALL
	DIVIDE EXTENDED FORMAT
*/

#include "FP_bias.h"
#include "FP_trap.h"
#include "FP_types.h"

/*
	November 15, 1984

	This is a routine to do the work.
	It is based on the partial products method
	and makes no use possible machine instructions
	to divide (hardware dividers).	It is intended
	that it be rewritten to do so, but expedieancy
	requires that something be written NOW - and
	this is it.
*/
/********************************************************/

div_ext(e1,e2)
EXTEND	*e1,*e2;
{
			short	count;
			short	error = 0;
			unsigned long	result[2];
	register	unsigned long	*lp;

#ifdef	PRT_EXT
	fprintf("stderr:start div_ext:\n");
	prt_ext("dividend:",e1);
	prt_ext("divisor :",e2);
#endif
	if ((e1->m1 | e1->m2) == 0) {	/* 0 / anything == 0 */
		e1->exp = 0;	/* make sure */
		return;
	}
	/*
	 * numbers are right shifted one bit to make sure
	 * that m1 is quaranteed to be larger if its
	 * maximum bit is set
	 */
	b64_sft(&e1->m1,1);	/* 64 bit shift right */
	b64_sft(&e2->m1,1);	/* 64 bit shift right */
	e1->exp++;
	e2->exp++;
	/*	check for underflow, divide by zero, etc	*/
	e1->sign ^= e2->sign;
	e1->exp -= e2->exp;
	e1->exp += 2;		/* bias correction	*/
	if (e1->exp < EXT_MIN)	{
		error++;
#ifdef	PRT_EXT
		prt_ext("DIV_EXT UNDERFLOW",e1);
#endif	PRT_EXT
		trap(EFUNFL);	/* underflow */
		e1->exp = EXT_MIN;
		e1->m1 = e1->m2 = 0L;
	}
	if ((e2->m1 | e2->m2) == 0) {
		error++;
#ifdef	PRT_EXT
		prt_ext("DIV_EXT DIV 0.0",e2);
#endif	PRT_EXT
		trap(EFDIVZ);
		e1->m1 = e1->m2 = 0L;
		e1->exp = EXT_MAX;
	}
	if (error)
		return;

		/* do division of mantissas	*/
		/* uses partial product method	*/
		/* init control variables	*/

	count = 64;
	lp = result;	/* result[0] == high word	*/
			/* result[0] == low  word	*/
	*lp++ = 0L;		/* high word	*/
	*lp-- = 0L;		/* low word	*/

		/* partial product division loop */

	while (count--)	{
		/* first left shift result 1 bit	*/
		/* this is ALWAYS done			*/

		b64_sft(result,-1);

		/* compare dividend and divisor		*/
		/* if dividend >= divisor add a bit	*/
		/* and subtract divisior from dividend	*/
#ifdef	PRT_ALL
	prt_ext("dividend:",e1);
	prt_ext("divisor :",e2);
#endif

		if ( (e1->m1 < e2->m1) ||
			((e1->m1 == e2->m1) && (e1->m2 < e2->m2) ))
			;	/* null statement */
				/* i.e., don't add or subtract */
		else	{
			result[1]++;	/* ADD	*/
			if (e2->m2 > e1->m2)
				e1->m1 -= 1;	/* carry in */
			e1->m1 -= e2->m1;	/* do SUBTRACTION */
			e1->m2 -= e2->m2;	/*    SUBTRACTION */
#ifdef	PRT_ALL
	prt_ext("result  :",e1);
#endif
		}
#ifdef	PRT_ALL
	fprintf(stderr,"div_ext %d %08X%08X\n\n",64-count,
		result[0],result[1]);
	fflush(stderr);
#endif

		/*	shift dividend left one bit OR	*/
		/*	IF it equals ZERO we can break out	*/
		/*	of the loop, but still must shift	*/
		/*	the quotient the remaining count bits	*/
		/* NB	save the results of this test in error	*/
		/*	if not zero, then the result is inexact. */
		/* 	this would be reported in IEEE standard	*/

		/*	lp points to dividend			*/
		lp = &e1->m1;

		error = ((*lp | *(lp+1)) != 0L) ? 1 : 0;
		if (error)	{	/* more work */
			/*	assume max bit == 0 (see above)	*/
			b64_sft(&e1->m1,-1);
			continue;
		}
		else
			break;	/* leave loop	*/
	}	/* end of divide by subtraction loop	*/

	/*	DISPLAY RESULTS FOR DEBUGGING		*/
#ifdef	PRT_ALL
	prt_ext("dividend:",e1);
	prt_ext("divisor :",e2);
	fprintf(stderr,"div_ext %d %08X%08X\n",64-count,
		result[0],result[1]);
#endif

	if (count > 0)	{
		lp = result;
		if (count > 31) {	/* move to higher word */
			*lp = *(lp+1);
			count -= 32;
			*(lp+1) = 0L;	/* clear low word	*/
		}
		if (*lp)
			*lp <<= count;	/* shift rest of way	*/
		lp++;	/*  == &result[1]	*/
		if (*lp) {
			result[0] |= (*lp >> 32-count);
			*lp <<= count;
		}
	}
	/*
	if (error)
		INEXACT();
	*/
	e1->m1 = result[0];
	e1->m2 = result[1];
#ifdef	PRT_EXT
	prt_ext("result  :",e1);
#endif
	nrm_ext(e1);
#ifdef	PRT_EXT
	prt_ext("after nrm:",e1);
	/*sleep(4);*/
#endif
}
