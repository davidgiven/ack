/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* btscat()
*/

#include "ack_string.h"

char *
btscat(b1, n1, b2, n2)
	char *b1;
	int n1;
	register char *b2;
	register int n2;
{
	register char *b = b1 + n1;

	while (n2-- > 0)
		*b++ = *b2++;
	return b1;
}
