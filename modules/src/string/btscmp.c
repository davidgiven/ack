/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* btscmp()
*/

#include "ack_string.h"


int btscmp(register char *s1, int n1, register char *s2, int n2)
{
	register int n = (n1 <= n2) ? n1 : n2;

	while (n-- > 0) {
		if (*s1++ != *s2++)
			return *--s1 - *--s2;
	}
	return n2 - n1;
}
