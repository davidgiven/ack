/*
 * perror.c - print an error message on the standard error output
 */
/* $Id$ */

#include <errno.h>
#include <stdio.h>
#include <string.h>

void perror(const char* s)
{
	if (s && *s)
	{
		(void)fputs(s, stderr);
		(void)fputs(": ", stderr);
	}
	(void)fputs(strerror(errno), stderr);
	(void)fputs("\n", stderr);
}
