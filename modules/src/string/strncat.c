/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* append t to s, upto n characters	
*/

#include "ack_string.h"

char *
strncat(s, t, n)
	register char *s;
	register _CONST char *t;
	register _SIZET n;
{
	register char *b = s;

	while (*s++)
		;
	s--;
	while ((n-- > 0) && (*s++ = *t++))
		;
	return b;
}
