/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

# include "adder.h"

b64_sft(e1,n)
B64	*e1;
short	n;
{
	if (n>0) do	{	/* RIGHT shift n bits */
		e1->l_32 >>= 1;		/* shift 64 bits	*/
		if (e1->h_32 & 1)
			e1->l_32 |= 0x80000000L;
		e1->h_32 >>= 1;
	} while	(--n);
	else			/* LEFT shift n bits	*/
		while (n++)	{
		e1->h_32 <<= 1;		/* shift 64 bits	*/
		if (e1->l_32 & 0x80000000L)
			e1->h_32 |= 1;
		e1->l_32 <<= 1;
	}
}
