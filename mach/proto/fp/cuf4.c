/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	CONVERT INTEGER TO FLOAT (CUF n 4)

	THIS ROUTINE WORKS BY FILLING AN EXTENDED
	WITH THE INTEGER VALUE IN EXTENDED FORMAT
	AND USES COMPACT() TO PUT IT INTO THE PROPER
	FLOATING POINT PRECISION.
*/

#include "FP_types.h"

_float
cuf4(ss,src)
int	ss;	/* source size */
long	src;	/* largest possible integer to convert */
{
	EXTEND	buf;
	short	*ipt;
	_float	*result;
	long	i_src;

	zrf_ext(&buf);
	if (ss == sizeof(long))	{
		buf.exp = 31;
		i_src = src;
		result = (_float *) &src;
	}
	else	{
		ipt = (short *) &src;
		i_src = (long) *ipt;
		buf.exp = 15;
		result = (_float *) &ss;
	}
	if (i_src == 0)	{
		*result = (_float) 0L;
		return (_float) 0L;
	}
			/* ESTABLISHED THAT src != 0	*/

			/* adjust exponent field	*/
	if (ss != sizeof(long))
		i_src <<= 16;

			/* move to mantissa field	*/
	buf.m1 = i_src;

			/* adjust mantissa field	*/
	nrm_ext(&buf);
	compact(&buf,(_double *) result,4);
	return *result;
}
