/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>
#include	<limits.h>

int
mblen(const char *s, size_t n)
{
	if (s == (char *)NULL)
		return 0;
	if (*s == '\0')
		return 0;
	if (n < 1 || n > MB_CUR_MAX)
		return -1;
	return MB_LEN_MAX;
}
