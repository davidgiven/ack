/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
#define	PRT_EXT
	SHIFT TWO EXTENDED NUMBERS INTO PROPER
	ALIGNMENT FOR ADDITION (exponents are equal)
*/

#include "FP_types.h"

sft_ext(e1,e2)
EXTEND	*e1,*e2;
{
	register	EXTEND	*s;
	register	short	diff;
			long	tmp;

#ifdef	PRT_EXT
	prt_ext("enter sft_ext e1:",e1);
	prt_ext("enter sft_ext e2:",e2);
#endif	PRT_EXT
	diff = e1->exp - e2->exp;

	if (!diff)
		return;	/* exponents are equal	*/

	if (diff < 0)	{ /* e2 is larger	*/
			/* shift e1		*/
		diff = -diff;
		s = e1;
	}
	else		/* e1 is larger		*/
			/* shift e2		*/
		s = e2;

	s->exp += diff;

	if (diff > 63)	{	/* no relative value */
		s->m1 = 0L;
		s->m2 = 0L;
		return;
	}

	if (diff > 32)	{
		diff -= 32;
		s->m2 = s->m1;
		s->m1 = 0L;
	}
	if (diff)	{
		if (s->m1)	{
			tmp = s->m1;
			tmp <<= (32-diff);
			s->m1 >>= diff;
		}
		else
			tmp = 0L;
		s->m2 >>= diff;
		s->m2 |= tmp;
	}
#ifdef	PRT_EXT
	prt_ext("exit  sft_ext e1:",e1);
	prt_ext("exit  sft_ext e2:",e2);
#endif	PRT_EXT
}
