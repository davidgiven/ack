/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	Access to program arguments and environment
  Author:	Ceriel J.H. Jacobs
  Version:	$Header$
*/

extern char **_argv, **_environ;
extern int _argc;
unsigned int Arguments_Argc;

static char *
findname(s1, s2)
register char *s1, *s2;
{

	while (*s1 == *s2++) s1++;
	if (*s1 == '\0' && *(s2-1) == '=') return s2;
	return 0;
}

static unsigned int
scopy(src, dst, max)
	register char *src, *dst;
	unsigned int max;
{
	register unsigned int i = 0;

	while (*src && i < max) {
		i++;
		*dst++ = *src++;
	}
	if (i <= max) {
		*dst = '\0';
		return i+1;
	}
	while (*src++) i++;
	return i + 1;
}

Arguments()
{
	Arguments_Argc = _argc;
}

unsigned
Arguments_Argv(n, argument, l, u, s)
	unsigned int u;
	char *argument;
{

	if (n >= _argc) return 0;
	return scopy(_argv[n], argument, u);
}

unsigned
Arguments_GetEnv(name, nn, nu, ns, value, l, u, s)
	char *name, *value;
	unsigned int nu, u;
{
	register char **p = _environ;
	register char *v = 0;

	while (*p && !(v = findname(name, *p++))) {
		/* nothing */
	}
	if (!v) return 0;
	return scopy(v, value, u);
}
