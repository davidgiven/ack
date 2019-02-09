/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* btscat()
*/

#include "ack_string.h"



char *btscat(char *s1, int n1, register char *s2, register int n2)
{
	register char *b = s1 + n1;

	while (n2-- > 0)
		*b++ = *s2++;
	return s1;
}
