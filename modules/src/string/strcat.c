/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* append t to s
*/

#include "ack_string.h"

char *
strcat(s, t)
	register char *s;
	register _CONST char *t;
{
	register char *b = s;

	while (*s++)
		;
	s--;
	while (*s++ = *t++)
		;
	return b;
}
