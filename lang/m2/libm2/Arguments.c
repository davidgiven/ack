/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	Access to program arguments and environment
  Author:	Ceriel J.H. Jacobs
  Version:	$Id$
*/
#include "libm2.h"

extern char **argv, **environ;
extern int argc;
unsigned int _Arguments__Argc;

static char* findname(char* s1, char* s2)
{

	while (*s1 == *s2++)
		s1++;
	if (*s1 == '\0' && *(s2 - 1) == '=')
		return s2;
	return 0;
}

static unsigned int scopy(char* src, char* dst, unsigned int max)
{
	register unsigned int i = 0;

	while (*src && i <= max)
	{
		i++;
		*dst++ = *src++;
	}
	if (i <= max)
	{
		*dst = '\0';
		return i + 1;
	}
	while (*src++)
		i++;
	return i + 1;
}

void _Arguments_(void)
{
	_Arguments__Argc = argc;
}

unsigned int _Arguments__Argv(int n, char* argument, int l, unsigned int u, int s)
{

	if (n >= argc)
		return 0;
	return scopy(argv[n], argument, u);
}

unsigned int _Arguments__GetEnv(
    char* name, int nn, unsigned int nu, int ns, char* value, int l, unsigned int u, int s)
{
	register char** p = environ;
	register char* v = 0;

	while (*p && !(v = findname(name, *p++)))
	{
		/* nothing */
	}
	if (!v)
		return 0;
	return scopy(v, value, u);
}
