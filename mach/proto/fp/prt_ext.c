/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/********************************************************/
/*
	PRINT EXTENDED FORMAT AND MESSAGE
	DEBUG ROUTINE
*/
/********************************************************/

#include "FP_types.h"

prt_ext(m,e)
char	*m;
EXTEND	*e;
{
#ifdef	PRT_EXT
	fprintf(stderr,"%s ",m);
	fprintf(stderr,"%c",(e->sign) ? '-' : '+');
	fprintf(stderr,"m1:0x%08X m2:0x%08X ^ %03d 0x%x\n",
		e->m1,e->m2,e->exp,e->exp);
	fprintf(stderr,"hit any key\n\r");
	fflush(stderr);
	getchar();
#endif
}
