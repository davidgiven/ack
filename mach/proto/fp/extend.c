/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
#define	PRT_EXIT
#define	PRT_ENTRY
#define	PRT_DBL
	CONVERTS FLOATING POINT TO EXTENDED FORMAT

	Two sizes of FLOATING Point are known:
		SINGLE and DOUBLE
*/
/********************************************************/
/*
	It is not required to normalize in extended
	format, but it has been chosen to do so.
	Extended Format is as follows (at exit):

->sign	S000 0000 | 0000 0000		<SIGN>
->exp	0EEE EEEE | EEEE EEEE		<EXPONENT>
->m1	LFFF FFFF | FFFF FFFF		<L.Fraction>
	FFFF FFFF | FFFF FFFF		<Fraction>
->m2	FFFF FFFF | FFFF FFFF		<Fraction>
	FFFF F000 | 0000 0000		<Fraction>
*/
/********************************************************/

#include "FP_bias.h"
#include "FP_shift.h"
#include "FP_types.h"
#include "get_put.h"
/********************************************************/

extend(from,to,size)
_double	*from;
EXTEND	*to;
int	size;
{
	DOUBLE	*f;
	register char *cpt1,*cpt2;
	unsigned long	tmp;
	int	leadbit = 0;

#ifdef	PRT_ENTRY
	write(2,"entry extend: ",14);
#ifdef	PRT_DBL
	prt_dbl(from,size);
#else
	write(2,"\n",1);
#endif	PRT_DBL
#endif	PRT_ENTRY
	f = (DOUBLE *) from;	/* local cast conversion */
	if (f->_s.p1.fract == 0L) 	{
		if (size == sizeof(SINGLE))	{
zero:			zrf_ext(to);
			goto ready;
		}
		else if (f->_s.p2 == 0L)
			goto zero;
	}
/*	there is a number to convert so lets get started	*/
/*	first extract the exponent; its always in the first two bytes */

	cpt1 = (char *) from;
	to->exp = uget2(cpt1);
	to->sign = (to->exp & 0x8000);	/* set sign bit */
	to->exp ^= to->sign;
	if (size == sizeof(DOUBLE))
		to->exp >>= DBL_EXPSHIFT;
	else
		to->exp >>= SGL_EXPSHIFT;
	if (to->exp > 0)
		leadbit++;	/* will set Lead bit later	*/

	to->m1 = get4(cpt1);

	if (size == sizeof(DOUBLE))	{
		to->m1 <<= DBL_M1LEFT;		/* shift	*/
		to->exp -= DBL_BIAS;		/* remove bias	*/
		cpt1 += 4;
		tmp = get4(cpt1);
		to->m1 |= (tmp>>DBL_RPACK);	/* plus 10 == 32	*/
		to->m2 = (tmp<<DBL_LPACK);	/* plus 22 == 32	*/
	}
	else	{	/* size == sizeof(SINGLE)		*/
		to->m1  <<= SGL_M1LEFT;	/* shift	*/
		to->exp -= SGL_BIAS;		/* remove bias	*/
		to->m2 = 0L;
	}

	to->m1 |= NORMBIT;				/* set bit L	*/
	if (leadbit == 0)		/* set or clear Leading Bit	*/
		to->m1 &= ~NORMBIT;			/* clear bit L	*/
ready:
#ifdef	PRT_EXIT
	prt_ext("exit extend:",to)
#endif	PRT_EXIT
	;	/* end of print statement or null statement */
}
