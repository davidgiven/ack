/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
		CONVERT DOUBLE TO FLOAT

	This routine works quite simply. A floating point
	of size 08 is converted to extended format.
	This extended variable is converted back to
	a floating point of size 04.

*/

#include	"FP_types.h"

cff4(src)
_double	src;	/* the source itself -	THIS TIME it's DOUBLE */
{
	EXTEND	buf;

	extend(&src,&buf,8);	/* no matter what */
#ifdef	PRT_EXT
	prt_ext("CFF4() entry:",&buf);
	fprintf(stderr,"ds(%d),ss(%d),src(%08X%08X)\n",8,4,src.__double[0],
							   src.__double[1]);
#endif	PRT_EXT
	compact(&buf,(_double *) &(src.__double[1]),4);
#ifdef	PRT_EXT
	fprintf(stderr,"CFF4() exit : %08X\n",src.__double[1]);
#endif	PRT_EXT
}
