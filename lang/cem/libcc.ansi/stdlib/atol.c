/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>
#include	<errno.h>

long
atol(const char *nptr)
{
	long l;
	int e = errno;

	l = strtol(nptr, (char **)NULL, 10);
	errno = e;
	return l;
}
