/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	SUBTRACT 2 EXTENDED FORMAT NUMBERS
*/
        /*
         * adf (addition routines) use this rather than
         * add_ext when the signs of the numbers are different.
         * sub_ext requires that e1 >= e2 on entry
         * otherwise nonsense results. If you use this routine
         * make certain this requirement is met.
         */

#include "FP_types.h"
sub_ext(e1,e2)
EXTEND	*e1,*e2;
{
	if (e2->m2 > e1->m2)
		e1->m1 -= 1;	/* carry in */
	e1->m1 -= e2->m1;
	e1->m2 -= e2->m2;
	nrm_ext(e1);
}
