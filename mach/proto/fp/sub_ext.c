/*
#define	PRT_EXT
	SUBTRACT EXTENDED FORMAT
*/
	/*	assumes that e1 >= e2 on entry	*/
	/*	no test is made to check this	*/
	/*	so make sure yourself		*/

#include "FP_types.h"
sub_ext(e1,e2)
EXTEND	*e1,*e2;
{
#ifdef	PRT_EXT
	prt_ext("before SUB_EXT() e1:",e1);
	prt_ext("before SUB_EXT() e2:",e2);
#endif	PRT_EXT
	if (e2->m2 > e1->m2)
		e1->m1 -= 1;	/* carry in */
	e1->m1 -= e2->m1;
	e1->m2 -= e2->m2;
#ifdef	PRT_EXT
	prt_ext("after  SUB_EXT() e1:",e1);
#endif	PRT_EXT
	nrm_ext(e1);
#ifdef	PRT_EXT
	prt_ext("after  NRM_EXT() e1:",e1);
#endif	PRT_EXT
}
