/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* return length of s
*/

#include "ack_string.h"

_SIZET
strlen(s)
	_CONST char *s;
{
	register _CONST char *b = s;

	while (*b++)
		;
	return b - s - 1;
}
