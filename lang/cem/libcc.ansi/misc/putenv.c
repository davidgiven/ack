/*
 * (c) copyright 1989 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include	<stdlib.h>
#include	<string.h>

#define	ENTRY_INC	10
#define	rounded(x)	(((x / ENTRY_INC) + 1) * ENTRY_INC)

extern const char **_penvp;
extern const char **environ;	/* environ is a shadow name for _penvp */

int
putenv(char *name)
{
	register const char **v = _penvp;
	register char *r;
	static int size = 0;
	/* When size != 0, it contains the number of entries in the
	 * table (including the final NULL pointer). This means that the
	 * last non-null entry  is _penvp[size - 2].
	 */

	if (!name) return 0;
	if (r = strchr(name, '=')) {
		register const char *p, *q;

		*r = '\0';

		if (v != NULL) {
			while ((p = *v) != NULL) {
				q = name;
				while (*q && (*q++ == *p++))
					/* EMPTY */ ;
				if (*q || (*p != '=')) {
					v++;
				} else {
					/* The name was already in the
					 * environment.
					 */
					*r = '=';
					*v = name;
					return 0;
				}
			}
		}
		*r = '=';
		v = _penvp;
	}

	if (!size) {
		register const char **p;
		register int i = 0;

		if (v)
			do {
				i++;
			} while (*v++);
		if (!(v = malloc(rounded(i) * sizeof(char **))))
			return 1;
		size = i;
		p = _penvp;
		_penvp = v;
		while (*v++ = *p++);		/* copy the environment */
		v = _penvp;
	} else if (!(size % ENTRY_INC)) {
		if (!(v = realloc(_penvp, rounded(size) * sizeof(char **))))
			return 1;
		_penvp = v;
	}
	v[size - 1] = name;
	v[size] = NULL;
	size++;
	environ = _penvp;
	return 0;
}
