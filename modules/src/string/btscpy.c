/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* btscpy()
*/

#include "ack_string.h"




char *btscpy(register char *s1, register char *s2, register int n)

{
	char *b1s = s1;

	while (n-- > 0)
		*s1++ = *s2++;
	return b1s;
}
