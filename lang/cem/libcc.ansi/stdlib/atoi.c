/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>
#include	<errno.h>

int
atoi(const char *nptr)
{
	int i, e = errno;

	i = (int)strtol(nptr, (char **)NULL, 10);
	errno = e;
	return i;
}
