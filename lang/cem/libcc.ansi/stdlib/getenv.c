/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include <string.h>

extern char* _findenv(const char* name, int* offset);

/*
 * getenv(name) --
 *	Returns ptr to value associated with name, if any, else NULL.
 */
char* getenv(const char* name)
{
    int offset;

    return(_findenv(name,&offset));
}

/*
 * _findenv(name,offset) --
 *	Returns pointer to value associated with name, if any, else NULL.
 *	Sets offset to be the offset of the name/value combination in the
 *	environmental array, for use by setenv(3) and unsetenv(3).
 *	Explicitly removes '=' in argument name.
 *
 *	This routine *should* be a static; don't use it.
 */
char* _findenv(register const char* name, int* offset)
{
    extern char **environ;
    register int    len;
    register char **P;
    register const char *C;

	if (!environ)
		return NULL;

    for (C = name,len = 0;*C && *C != '=';++C,++len);
    for (P = environ;*P;++P)
        if (!strncmp(*P,name,len))
            if (*(C = *P + len) == '=') {
                *offset = P - environ;
                return (char*)(++C);
            }
    return(NULL);
}

