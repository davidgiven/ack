/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>

extern char **environ;

char *
getenv(const char *name)
{
	register char **v = environ;
	register char *p, *q;

	if (v == (char **)NULL || name == (char *)NULL)
		return (char *)NULL;
	while ((p = *v++) != (char *)NULL) {
		q = name;
		while (*q && (*q++ == *p++))
			/* EMPTY */ ;
		if (*q || (*p != '='))
			continue;
		return(p+1);
	}
	return((char *)NULL);
}
