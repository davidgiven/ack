/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	CONVERT INTEGER TO FLOAT

	THIS ROUTINE WORKS BY FILLING AN EXTENDED
	WITH THE INTEGER VALUE IN EXTENDED FORMAT
	AND USES COMPACT() TO PUT IT INTO THE PROPER
	FLOATING POINT PRECISION.
*/

#include "FP_types.h"

cuf8(ss,src)
int	ss;	/* source size */
long	src;	/* largest possible integer to convert */
{
	EXTEND	buf;
	short	*ipt;
	long	i_src;

	zrf_ext(&buf);
	if (ss == sizeof(long))	{
		buf.exp = 33;
		i_src = src;
	}
	else	{
		ipt = (short *) &src;
		i_src = (long) *ipt;
		buf.exp = 17;
	}
#ifdef	PRT_STDERR
	fprintf(stderr,"CUF8(ds(%d),ss(%d),src(%D))\n\n",8,ss,i_src);
#endif
	if (i_src == 0)	{
		zrf8(&ss);
		return;
	}
			/* ESTABLISHED THAT src != 0	*/

			/* adjust exponent field	*/
	if (ss != sizeof(long))
		i_src <<= 16;

			/* move to mantissa field	*/
	buf.m1 = i_src;

			/* adjust mantissa field	*/
	nrm_ext(&buf);
#ifdef	PRT_STDERR
	fprintf(stderr,"CUF() buf.exp after nrm_ext() == %d\n\n",buf.exp);
#endif
	compact(&buf,(_double *) &ss,8);
}
