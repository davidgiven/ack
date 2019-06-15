/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "ack_string.h"

char *strrindex(register char *str, int chr)
{
	register char *retptr = 0;

	while (*str)
		if (*str++ == chr)
			retptr = &str[-1];
	return retptr;
}
