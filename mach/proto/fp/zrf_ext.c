/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	ZERO and return EXTEND FORMAT FLOAT
*/

#include "FP_types.h"

zrf_ext(e)
EXTEND	*e;
{
	register short *ipt;
	register int  i;

	/*	local CAST conversion	*/
	ipt = (short *) e;

	i = sizeof(EXTEND)/sizeof(short);
	while (i--)
		*ipt++ = 0;
}
