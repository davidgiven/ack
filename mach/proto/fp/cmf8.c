/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
	COMPARE	DOUBLES
*/

#include	"FP_types.h"
#include	"get_put.h"

short
cmf8(d1,d2)
_double	d1,d2;
{
#define	SIGN(x)	(((x) < 0) ? -1 : 1)
		/*
		 * return ((d1 < d2) ? 1 : (d1 > d2) ? -1 : 0))
		 */
	long	l1,l2;
	unsigned short	*s1,*s2;
	int	sign1,sign2;

	l1 = get4((char *)&d1);
	l2 = get4((char *)&d2);
	sign1 = SIGN(l1);
	sign2 = SIGN(l2);
	if (sign1 != sign2)
		return ((sign1 > 0) ? -1 : 1);
	if (l1 != l2)	{	/* we can decide here */
		s1 = (unsigned short *) &l1;
		s2 = (unsigned short *) &l2;
					/* set both signs positive */
		*s1 &= ~0x8000;
		*s2 &= ~0x8000;
				/* we already know they aren't equal so */
		return (sign1 * ((l1 < l2) ? 1 : -1));
	}
	else	{ 		/* decide in 2nd half */
	l1 = get4(((char *)&d1 + 4));
	l2 = get4(((char *)&d2 + 4));
		if (l1 == l2)
			return(0);
		else	{
			s1 = (unsigned short *) &l1;
			s2 = (unsigned short *) &l2;
			if (*s1 == *s2)	{
				s1++;
				s2++;
			}
			return (sign1 * ((*s1 < *s2) ? 1 : -1));
		}
	}
}
