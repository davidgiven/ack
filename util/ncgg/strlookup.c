/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include "param.h"

int nstrings=0;
char *l_strings[MAXSTRINGS];

strlookup(str) char *str; {
	register i;
	extern char *mystrcpy();

	for(i=0;i<nstrings;i++)
		if (strcmp(str,l_strings[i])==0)
			return(i);
	NEXT(nstrings,MAXSTRINGS,"String table");
	l_strings[i] = mystrcpy(str);
	return(i);
}
