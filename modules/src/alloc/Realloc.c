/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	M E M O R Y  A L L O C A T I O N  R O U T I N E S	*/

/*	The memory allocation routines offered in this file are:

	char *Realloc(ptr, n)	: reallocate buffer to n bytes
*/

#include	"alloc.h"

char *
Realloc(ptr, sz)
	char ptr[];
	unsigned int sz;
{
	register char *mptr = realloc(ptr, sz);
	if (mptr == 0) No_Mem();
	return mptr;
}
