/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* strzero()
*/

#include "ack_string.h"

char *
strzero(s)
	char *s;
{
	*s = '\0';
	return s;
}
