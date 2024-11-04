/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* btscat()
*/

#include "ack_string.h"

char *btscat(char* b1, int n1, char *b2, int n2)
{
	char *b = b1 + n1;

	while (n2-- > 0)
		*b++ = *b2++;
	return b1;
}
