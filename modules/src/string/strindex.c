/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* strindex() -- (86/03/18 EHB)
*/

#include "ack_string.h"

char *
strindex(s, c)
	register char *s;
	int c;
{
	while (*s)
		if (*s++ == c)
			return --s;
	return (char *)0;
}
