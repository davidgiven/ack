/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
		CONVERT FLOAT TO DOUBLE (CFF 4 8)

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
	compact(&buf,&src,8);
}
