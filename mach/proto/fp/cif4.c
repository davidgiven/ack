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

_float
cif4(ss,src)
int	ss;	/* source size */
long	src;	/* largest possible integer to convert */
{
	EXTEND	buf;
	short	*ipt;
	long	i_src;
	_float	*result;

	zrf_ext(&buf);
	if (ss == sizeof(long))	{
		buf.exp = 33;
		i_src = src;
		result = (_float *) &src;
	}
	else	{
		ipt = (short *) &src;
		i_src = (long) *ipt;
		buf.exp = 17;
		result = (_float *) &ss;
	}
#ifdef	PRT_STDERR
	fprintf(stderr,"CIF4(ds(%d),ss(%d),src(%D))\n\n",4,ss,i_src);
#endif
	if (i_src == 0)	{
		*result = (_float) 0L;
		return(0L);
	}
			/* ESTABLISHED THAT src != 0	*/
			/* adjust exponent field	*/
	buf.sign = (i_src < 0) ? 0x8000 : 0;
			/* clear sign bit of integer	*/
			/* move to mantissa field	*/
	buf.m1 = (i_src < 0) ? -i_src : i_src;
			/* adjust mantissa field	*/
	if (ss != sizeof(long))
		buf.m1 <<= 16;
	nrm_ext(&buf);		/* adjust mantissa field	*/
#ifdef	PRT_STDERR
	fprintf(stderr,"CIF() buf.exp after nrm_ext() == %d\n\n",buf.exp);
#endif
	compact(&buf,(_double *) result,4);	/* put on stack */
	return(*result);
}
