/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
		CONVERT FLOAT TO DOUBLE

	This routine works quite simply. A floating point
	of size 04 is converted to extended format.
	This extended variable is converted back to
	a floating point of size 08.

*/

#include "FP_types.h"

cff8(src)
_float	src;	/* the space on the stack is for a double - see cg/table */
{
	EXTEND	buf;

	extend((_double *) &src,&buf,4);	/* no matter what */
#ifdef	PRT_EXT
	prt_ext("CFF8() entry:",&buf);
	fprintf(stderr,"ds(%d),ss(%d),src(%08X)\n",4,8,src);
#endif
	compact(&buf,&src,8);
#ifdef	DEBUG
	fprintf(stderr,"CFF8() exit : %08X",src.__double[0]);
	fprintf(stderr,"%08X\n",src.__double[1]);
#endif	DEBUG
}
