/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	return negative, zero or positive value if
	resp. s < t, s == t or s > t
*/

#include "ack_string.h"

int
strcmp(s, t)
	register _CONST char *s, *t;
{
	while (*s == *t++)
		if (*s++ == '\0')
			return 0;
	return *s - *--t;
}
