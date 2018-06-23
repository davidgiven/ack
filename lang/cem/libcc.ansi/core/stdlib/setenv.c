/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include <string.h>

extern char* _findenv(const char* name, int* offset);
extern char** environ;

/*
 * setenv(name,value,rewrite)
 *	Set the value of the environmental variable "name" to be
 *	"value".  If rewrite is set, replace any current value.
 */
int setenv(register const char* name, register const char* value, int rewrite)
{
	static int alloced = 0; /* if allocated space before */
	register char* C;
	int l_value,
	    offset;

	if (*value == '=') /* no `=' in value */
		++value;
	l_value = strlen(value);
	if ((C = _findenv(name, &offset)))
	{ /* find if already exists */
		if (!rewrite)
			return (0);
		if (strlen(C) >= l_value)
		{ /* old larger; copy over */
			while (*C++ = *value++)
				;
			return (0);
		}
	}
	else
	{ /* create new slot */
		register int cnt = 0;
		register char** P;

		if (environ)
			for (P = environ; *P; ++P, ++cnt)
				;
		if (alloced)
		{ /* just increase size */
			environ = (char**)realloc((char*)environ,
			    (unsigned)(sizeof(char*) * (cnt + 2)));
			if (!environ)
				return (-1);
		}
		else
		{ /* get new space */
			alloced = 1; /* copy old entries into it */
			P = (char**)malloc((unsigned)(sizeof(char*) * (cnt + 2)));
			if (!P)
				return (-1);
			if (environ)
				bcopy(environ, P, cnt * sizeof(char*));
			environ = P;
		}
		environ[cnt + 1] = NULL;
		offset = cnt;
	}
	for (C = (char*) name; *C && *C != '='; ++C)
		; /* no `=' in name */
	if (!(environ[offset] = /* name + `=' + value */
	        malloc((unsigned)((int)(C - name) + l_value + 2))))
		return (-1);
	for (C = environ[offset]; (*C = *name++) && *C != '='; ++C)
		;
	for (*C++ = '='; *C++ = *value++;)
		;
	return (0);
}

/*
 * unsetenv(name) --
 *	Delete environmental variable "name".
 */
int unsetenv(const char* name)
{
	register char** P;
	int offset;

	while (_findenv(name, &offset)) /* if set multiple times */
		for (P = &environ[offset];; ++P)
			if (!(*P = *(P + 1)))
				break;

	return 0;
}
