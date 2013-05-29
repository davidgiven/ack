/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include	<string.h>

char*
strdup(const char *s)
{
	int len = strlen(s);
	char *p = malloc(len+1);
	if (p)
		memcpy(p, s, len+1);
	return p;
}
