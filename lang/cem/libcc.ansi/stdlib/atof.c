/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef	NOFLOAT
#include	<stdlib.h>

double
atof(const char *nptr)
{
	return strtod(nptr, (char **) NULL);
}
#endif	/* NOFLOAT */
