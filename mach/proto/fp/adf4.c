/*
	ADD TWO FLOATS - SINGLE
*/

#include	"FP_types.h"

_float
adf4(s2,s1)
_float	s1,s2;
{
	EXTEND	e1,e2;
	int	swap = 0;

	extend((_double *)&s1,&e1,sizeof(SINGLE));
	extend((_double *)&s2,&e2,sizeof(SINGLE));
			/* if signs differ do subtraction	*/
			/* result in e1 */
	if (e1.sign ^ e2.sign)	{
#ifdef	DEBUG
		printf("\t\t\tADF calls SUBTRACT\n");
#endif	DEBUG
			/* set sign of e1 to sign of largest	*/
		swap = (e2.exp >  e1.exp) ? 1 : (e2.exp < e1.exp) ? 0 :
		       (e2.m1  >  e1.m1 ) ? 1 : 0;
			/* adjust mantissas to equal powers */
		sft_ext(&e1,&e2);
			/* subtract the extended formats	*/
		if (swap)	{
#ifdef	DEBUG
			printf("\t\t\t\tSWAP\n");
#endif	DEBUG
			sub_ext(&e2,&e1);
			e1 = e2;
		}
		else
			sub_ext(&e1,&e2);
	}
	else	{
#ifdef	DEBUG
		printf("\t\t\tADF calls ADDITION\n");
#endif	DEBUG
			/* adjust mantissas to equal powers */
		sft_ext(&e1,&e2);
		add_ext(&e1,&e2);
	}
	compact(&e1,(_double *)&s1,sizeof(SINGLE));
	return(s1);
}
