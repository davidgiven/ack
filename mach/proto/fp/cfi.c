/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
		CONVERT FLOAT TO UNSIGNED

		N.B. The caller must know what it is getting.
		     A LONG is always returned. If it is an
		     integer the high byte is cleared first.
*/

#include "FP_trap.h"
#include "FP_types.h"

long
cfi(ds,ss,src)
int	ds;	/* destination size (2 or 4) */
int	ss;	/* source size	    (4 or 8) */
_double	src;	/* assume worst case */
{
	EXTEND	buf;
	long	new;
	short	newint,	max_exp;

	extend(&src,&buf,ss);	/* get extended format */
#ifdef	PRT_EXT
	prt_ext("CFI() entry:",&buf);
#endif	PRT_EXT
	buf.exp--;		/* additional bias correction */
	if (buf.exp < 1) {	/* no conversion needed */
		src.__double[ss == 8] = 0L;
		return(0L);
	}
	max_exp = (ds << 3) - 1;	/* signed numbers */
				/* have more limited max_exp */
	if (buf.exp > max_exp) {
#ifdef	PRT_EXT
		prt_ext("CFI() INT OVERFLOW", &buf);
#endif	PRT_EXT
		trap(EIOVFL);	/* integer overflow	*/
		buf.exp %= max_exp; /* truncate	*/
	}
	new = buf.m1 >> (32-buf.exp);
	if (buf.sign)
		new = -new;
done:
	src.__double[ss == 8] = new;
	return(new);
}
