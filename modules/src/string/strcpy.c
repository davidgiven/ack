/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* Copy t into s
*/

#include "ack_string.h"

char *
strcpy(s, t)
	register char *s;
	register _CONST char *t;
{
	register char *b = s;

	while (*s++ = *t++)
		;
	return b;
}
