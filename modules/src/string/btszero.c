/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* btszero()
*/

#include "ack_string.h"

char *
btszero(b, n)
	char *b;
	register int n;
{
	register char *s = b;

	while (n-- > 0)
		*s++ = '\0';
	
	return b;
}
