/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
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
	int	error = 0;

	if (size == sizeof(_double)) {
	/*
	 * COMPACT EXTENDED INTO DOUBLE
	 */
		DOUBLE *DBL;

		if ((f->m1|(f->m2 & DBL_ZERO)) == 0L)	{
			zrf8(to);
			return;
		}
		f->exp += DBL_BIAS;	/* restore proper bias	*/
		if (f->exp > DBL_MAX)	{
dbl_over:			trap(EFOVFL);
			f->exp = DBL_MAX+1;
			f->m1 = 0;
			f->m2 = 0;
			if (error++)
				return;
		}
		else if (f->exp < DBL_MIN)	{
			b64_rsft(&(f->m1));
			if (f->exp < 0) {
				b64_sft(&(f->m1), -f->exp);
				f->exp = 0;
			}
			/* underflow ??? */
		}
			
		/* local CAST conversion		*/
		DBL = (DOUBLE *) to;

		/* because of special format shift only 10 bits */
		/* bit shift mantissa 10 bits		*/

		/* first align within words, then do store operation */

		DBL->_s.p1.fract = f->m1 >> DBL_RUNPACK;   /* plus 22 == 32 */
		DBL->_s.p2       = f->m2 >> DBL_RUNPACK;   /* plus 22 == 32 */
		DBL->_s.p2      |= (f->m1 << DBL_LUNPACK); /* plus 10 == 32 */

		/* if not exact then round to nearest	*/

#ifdef EXCEPTION_INEXACT
		if ((f->m2 & DBL_EXACT) != 0) {
		    INEXACT();
#endif
		    if (f->m2 & DBL_ROUNDUP)	{
			DBL->_s.p2++;	/* rounding up	*/
			if (DBL->_s.p2 == 0L) { /* carry out	*/
			    DBL->_s.p1.fract++;

			    if (f->exp == 0 && (DBL->_s.p1.fract & ~DBL_MASK)) {
					f->exp++;
				}
			    if (DBL->_s.p1.fract & DBL_CARRYOUT) { /* carry out */
				if (DBL->_s.p1.fract & 01)
				    DBL->_s.p2 = CARRYBIT;
				DBL->_s.p1.fract >>= 1;
				f->exp++;
			    }
			}
			/*	check for overflow			*/
			if (f->exp > DBL_MAX)
		    		goto dbl_over;
		    }
#ifdef EXCEPTION_INEXACT
		}
#endif

		/*
		 * STORE EXPONENT AND SIGN:
		 *
		 * 1) clear leading bits (B4-B15)
		 * 2) shift and store exponent
		 */

		DBL->_s.p1.fract &= DBL_MASK;
		DBL->_s.p1.fract |= 
			((long) (f->exp << DBL_EXPSHIFT) << EXP_STORE);
		if (f->sign)
			DBL->_s.p1.fract |= CARRYBIT;

		/*
		 * STORE MANTISSA
		 */

#if FL_MSL_AT_LOW_ADDRESS
		put4(DBL->_s.p1.fract, (char *) &DBL->_s.p1.fract);
		put4(DBL->_s.p2, (char *) &DBL->_s.p2);
#else
		{ unsigned long l;
		  put4(DBL->_s.p2, (char *) &l);
		  put4(DBL->_s.p1.fract, (char *) &DBL->_s.p2);
		  DBL->_s.p1.fract = l;
		}
#endif
	}
	else {
		/*
		 * COMPACT EXTENDED INTO FLOAT
		 */
		SINGLE	*SGL;

		/* local CAST conversion		*/
		SGL = (SINGLE *) to;
		if ((f->m1 & SGL_ZERO) == 0L)	{
			SGL->fract = 0L;
			return;
		}
		f->exp += SGL_BIAS;	/* restore bias	*/
		if (f->exp > SGL_MAX)	{
sgl_over:			trap(EFOVFL);
			f->exp = SGL_MAX+1;
			f->m1 = 0L;
			f->m2 = 0L;
			if (error++)
				return;
		}
		else if (f->exp < SGL_MIN)	{
			b64_rsft(&(f->m1));
			if (f->exp < 0) {
				b64_sft(&(f->m1), -f->exp);
				f->exp = 0;
			}
			/* underflow ??? */
		}

		/* shift mantissa and store	*/
		SGL->fract = (f->m1 >> SGL_RUNPACK);

		/* check for rounding to nearest	*/
#ifdef EXCEPTION_INEXACT
		if (f->m2 != 0 ||
		    (f->m1 & SGL_EXACT) != 0L) {
			INEXACT();
#endif
			if (f->m1 & SGL_ROUNDUP) {
				SGL->fract++;
				if (f->exp == 0 && (f->m1 & ~SGL_MASK)) {
					f->exp++;
				}
			/* check normal */
				if (SGL->fract & SGL_CARRYOUT)	{
					SGL->fract >>= 1;
					f->exp++;
				}
				if (f->exp > SGL_MAX)
					goto sgl_over;
			}
#ifdef EXCEPTION_INEXACT
		}
#endif

		/*
		 * STORE EXPONENT AND SIGN:
		 *
		 * 1) clear leading bit of fraction
		 * 2) shift and store exponent
		 */

		SGL->fract &= SGL_MASK; /* B23-B31 are 0 */
		SGL->fract |= 
			((long) (f->exp << SGL_EXPSHIFT) << EXP_STORE);
		if (f->sign)
			SGL->fract |= CARRYBIT;

		/*
		 * STORE MANTISSA
		 */

		put4(SGL->fract, (char *) &SGL->fract);
	}
}
