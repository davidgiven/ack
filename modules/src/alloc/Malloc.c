/*	M E M O R Y  A L L O C A T I O N  R O U T I N E S	*/

/*	The memory allocation routines offered in this file are:

	char *Malloc(n)		: allocate n bytes
	char *Srealloc(ptr, n)	: reallocate buffer to n bytes
	char *Salloc(str, n)	: allocate n bytes, initialized with the string
					str

	This file imports routines from "system".
*/

#include	<system.h>
#include	"in_all.h"
#include	"alloc.h"

EXPORT char *
Malloc(sz)
	unsigned int sz;
{
	char *res = malloc(sz);
	
	if (res == 0) No_Mem();
	return res;
}

EXPORT char *
Salloc(str, sz)
	register char str[];
	register unsigned int sz;
{
	/*	Salloc() is not a primitive function: it just allocates a
		piece of storage and copies a given string into it.
	*/
	char *res = malloc(sz);
	register char *m = res;

	if (m == 0) No_Mem();
	while (sz--)
		*m++ = *str++;
	return res;
}

EXPORT char *
Srealloc(str, sz)
	char str[];
	unsigned int sz;
{
	str = realloc(str, sz);
	if (str == 0) No_Mem();
	return str;
}
