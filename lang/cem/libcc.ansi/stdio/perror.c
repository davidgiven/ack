/*
 * perror.c - print an error message on the standard error output
 */
/* $Header$ */

#include	<errno.h>
#include	<stdio.h>
#include	<string.h>

void
perror(const char *s)
{
	if (s && *s)
		(void) fprintf(stderr,"%s: ", s);
	(void) fprintf(stderr,"%s\n", strerror(errno));
}
