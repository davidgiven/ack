/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
#define	PRT_EXIT
#define	PRT_TRAP
#define	PRT_ENTRY
	COMPACT EXTEND FORMAT INTO FLOAT OF PROPER SIZE
*/

# include "FP_bias.h"
# include "FP_shift.h"
# include "FP_trap.h"
# include "FP_types.h"
# include "get_put.h"

compact(f,to,size)
EXTEND	*f;
_double	*to;
int	size;
{
	DOUBLE	*DBL;
	int	error = 0;
	SINGLE	*SGL;
	int	exact;

#ifdef	PRT_ENTRY
	prt_ext("enter compact:",f);
#endif	PRT_ENTRY
	if (size == sizeof(_double))
/********************************************************/
/*
	COMPACT EXTENDED INTO DOUBLE
*/
/********************************************************/
	{
		if ((f->m1|(f->m2 & DBL_ZERO)) == 0L)	{
			zrf8(to);
			goto leave;
		}
		f->exp += DBL_BIAS;	/* restore proper bias	*/
		if (f->exp > DBL_MAX)	{
dbl_over:			trap(EFOVFL);
#ifdef	PRT_TRAP
				prt_ext("FCOMPACT DBL OVERFLOW",f);
#endif	PRT_TRAP
			f->exp = DBL_MAX;
			f->m1 = f->m2 = 0L;
			if (error++)
				return;
		}
		else if (f->exp < DBL_MIN)	{
#ifdef	PRT_TRAP
			prt_ext("FCOMPACT DBL UNDERFLOW",f);
#endif	PRT_TRAP
			trap(EFUNFL);
			f->exp = DBL_MIN;
			f->m1 = f->m2 = 0L;
			if (error++)
				return;
		}
			
		/* local CAST conversion		*/
		DBL = (DOUBLE *) to;
		/* check if answer is exact		*/
		/* (the last 11 bits are zero (0)	*/

		exact = ((f->m2 & DBL_EXACT) == 0) ? 1 : 0;

		/* because of special format shift only 10 bits */
		/* bit shift mantissa 10 bits		*/

		/* first align within words, then do store operation */

		DBL->_s.p1.fract = f->m1 >> DBL_RUNPACK;   /* plus 22 == 32 */
		DBL->_s.p2       = f->m2 >> DBL_RUNPACK;   /* plus 22 == 32 */
		DBL->_s.p2      |= (f->m1 << DBL_LUNPACK); /* plus 10 == 32 */

		/* if not exact then round to nearest	*/

		if (!exact)	{
		    /* INEXACT(); */
		    if (f->m2 & DBL_ROUNDUP)	{
			DBL->_s.p2++;	/* rounding up	*/
			if (DBL->_s.p2 == 0L) { /* carry out	*/
#ifdef	PRT_RNDMSG
			    write(2,"rounding up lsb\n",16);
#endif	PRT_RNDMSG
			    DBL->_s.p1.fract++;
			    if (DBL->_s.p1.fract & DBL_CARRYOUT) { /* carry out */
#ifdef	PRT_RNDMSG
				write(2,"shift due to rounding\n",22);
#endif	PRT_RNDMSG
				if (DBL->_s.p1.fract & 01)
				    DBL->_s.p2 = CARRYBIT;
				DBL->_s.p1.fract >>= 1;
				f->exp++;
			    }
			}
		    }
		}
		/*	check for overflow			*/
		if (f->exp > DBL_MAX)
		    goto dbl_over;

		/* STORE EXPONENT:			*/

		/* 1) clear leading bits (B4-B15)	*/
		DBL->_s.p1.fract &= DBL_MASK;

		/* 2) shift and store exponent		*/
		f->exp <<= DBL_EXPSHIFT;
		DBL->_s.p1.fract |= ((long) f->exp << EXP_STORE);
	}
	else
/********************************************************/
/*
	COMPACT EXTENDED INTO FLOAT
*/
/********************************************************/
	{
		/* local CAST conversion		*/
		SGL = (SINGLE *) to;
		if ((f->m1 & SGL_ZERO) == 0L)	{
			SGL->fract = 0L;
			goto leave;
		}
		f->exp += SGL_BIAS;	/* restore bias	*/
		if (f->exp > SGL_MAX)	{
sgl_over:			trap(EFOVFL);
#ifdef	PRT_TRAP
				prt_ext("FCOMPACT FLOAT OVERFLOW",f);
#endif	PRT_TRAP
			f->exp = SGL_MAX;
			f->m1 = f->m2 = 0L;
			if (error++)
				return;
		}
		else if (f->exp < SGL_MIN)	{
#ifdef	PRT_TRAP
			prt_ext("FCOMPACT FLOAT UNDERFLOW",f);
#endif	PRT_TRAP
			trap(EFUNFL);
			f->exp = SGL_MIN;
			f->m1 = f->m2 = 0L;
			if (error++)
				return;
		}
		/* check if the answer is exact */
		/* the last 40 bits are zero	*/
		/* check first last bits of mantissa 1 */
		exact = ((f->m1 & SGL_EXACT) == 0) ? 1 : 0;

		/* check last 32 bits in mantissa 2	*/
		if (exact) /* first part masks to zero	*/
			exact = (f->m2 == 0L) ? 1 : 0;

		/* shift mantissa and store	*/
		SGL->fract = (f->m1 >> SGL_RUNPACK);

		/* check for rounding to nearest	*/
		if (!exact)	{
			/* INEXACT(); */
			if (f->m1 & SGL_ROUNDUP) {
				SGL->fract++;
#ifdef	PRT_RNDMSG
				write(2,"rounding up lsb\n",16);
#endif	PRT_RNDMSG
			/* check normal */
				if (SGL->fract & SGL_CARRYOUT)	{
					SGL->fract >>= 1;
					f->exp++;
				}
			}
		}
		if (f->exp > SGL_MAX)
			goto sgl_over;

		/* STORE EXPONENT			*/
		/* 1) clear leading bit of fraction	*/
		SGL->fract &= SGL_MASK; /* B23-B31 are 0 */

		/* 2) shift and store exponent		*/
		f->exp <<= SGL_EXPSHIFT;
		SGL->fract |= ((long) f->exp << EXP_STORE);
	}

/********************************************************/
/*
	STORE SIGN BIT
*/
/********************************************************/
	if (f->sign)	{
		SGL = (SINGLE *) to;	/* make sure	*/
		SGL->fract |= CARRYBIT;
	}
/********************************************************/
/*
	STORE MANTISSA
/*
/********************************************************/

	if (size == sizeof(_double)) {
		put4(DBL->_s.p1.fract, (char *) &DBL->_s.p1.fract);
		put4(DBL->_s.p2, (char *) &DBL->_s.p2);
	}
		else
		put4(SGL->fract, (char *) &SGL->fract);

leave:
#ifdef	PRT_EXIT
	prt_ext("exit  compact:",f);
	prt_dbl((DOUBLE *) to,size); getchar();
#endif	PRT_EXIT
	;	/* end of print statement or null statement */
}
