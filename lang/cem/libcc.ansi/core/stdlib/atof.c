/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include <errno.h>
#include <ack/config.h>

#if ACKCONF_WANT_FLOAT

double(atof)(const char* nptr)
{
	double d;
	int e = errno;

	d = strtod(nptr, (char**)NULL);
	errno = e;
	return d;
}

#endif
