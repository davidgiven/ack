/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	ADD TWO FLOATS - DOUBLE
*/

#include	"FP_types.h"

_double
adf8(s2,s1)
_double	s1,s2;
{
	EXTEND	e1,e2;
	short	swap;

	if (s1.__double[0] == 0 && s1.__double[1] == 0) {
		s1 = s2;
		return s1;
	}
	if (s2.__double[0] == 0 && s2.__double[1] == 0) {
		return s1;
	}

	extend(&s1,&e1,sizeof(_double));
	extend(&s2,&e2,sizeof(_double));
#ifdef	PRT_EXT
	prt_ext("ADF8: e1",&e1);
	prt_ext("ADF8: e2",&e2);
#endif
			/* adjust mantissas to equal powers */
	if (e1.sign ^ e2.sign)	{ /* signs are different */
			/* determine which is largest number	*/
		swap = (e2.exp >  e1.exp) ? 1 : (e2.exp < e1.exp) ? 0 :
		       (e2.m1  >  e1.m1 ) ? 1 : (e2.m1  < e1.m1 ) ? 0 :
		       (e2.m2  >  e1.m2 ) ? 1 : 0;
			/* adjust mantissas to equal powers */
		sft_ext(&e1,&e2);
			/* subtract the extended formats	*/
		if (swap)	{	/* &e2 is the largest number */
#ifdef	PRT_EXT
	fprintf(stderr,"ADF8: swaps and subtracts extended\n");
#endif
			sub_ext(&e2,&e1);
			e1 = e2;
		}
		else	{
			sub_ext(&e1,&e2);
		}
	}
	else	{
			/* adjust mantissas to equal powers */
		sft_ext(&e1,&e2);
		add_ext(&e1,&e2);
	}
#ifdef	PRT_EXT
	prt_ext("ADF8: e1 result",&e1);
#endif
	compact(&e1,&s1,sizeof(_double));
	return(s1);
}
