/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

size_t
strlen(register const char *s)
{
	const char *org = s;

	while (*s++)
		/* EMPTY */ ;

	return (s - 1) - org;
}
