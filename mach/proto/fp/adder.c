/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
 *	these are the routines the routines to do 32 and  64-bit addition
 */

# ifdef	EXT_DEBUG
# include <stdio.h>
# endif

# include "adder.h"
# define	UNKNOWN -1
# define	TRUE	 1
# define	FALSE	 0
# define	MAXBIT	0x80000000L

	/*
	 *	add 64 bits
	 */
b64_add(e1,e2)
		/*
		 * pointers to 64 bit 'registers'
		 */
register	B64	*e1,*e2;
{
		register	int	overflow;
				int	carry;

			/* add higher pair of 32 bits */
	overflow = b32_add(&e1->h_32,&e2->h_32);

			/* add lower pair of 32 bits */
	carry =	   b32_add(&e1->l_32,&e2->l_32);
# ifdef	EXT_DEBUG
	printf("\t\t\t\t\tb64_add: overflow (%d); internal carry(%d)\n",
					overflow,carry);
	fflush(stdout);
# endif
	if ((carry) && (++e1->h_32 == 0))
		return(TRUE);		/* had a 64 bit overflow */
	else
		return(overflow);	/* return status from higher add */
}

	/*
	 *	add 32 bits (unsigned longs)
	 *	and return the carry status
	 */

b32_add(e1,e2)
register	unsigned long	*e1,*e2;
{
	register	int	carry;

	if (*e1 & *e2 & MAXBIT) /* both max_bits are set */
		carry = TRUE;	 /* so there is a carry	*/
	else 
		carry = ((*e1 | *e2) & MAXBIT)
				/* only one is set - might be a carry */
			? UNKNOWN
				/* both are clear - no carry */
			: FALSE;
# ifdef EXT_DEBUG
	fflush(stdout);
	printf("\t\t\t\t\tb32_add: overflow before add(%d) test(%d)\n",
				carry,(*e1&MAXBIT)?FALSE:TRUE);
	printf("%08X\n%08X\n",*e1,*e2);
# endif

	*e1 += *e2;
# ifdef EXT_DEBUG
	printf("%08X\n",*e1);
	fflush(stdout);
# endif
	if (carry != UNKNOWN)
		return(carry);
	else
		/*
		 * if maxbit in answer is set there is no carry
		 * return the NAND of this bit
		 */
		return((*e1&MAXBIT)?FALSE:TRUE);
}
