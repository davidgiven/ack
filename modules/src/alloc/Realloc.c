/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	M E M O R Y  A L L O C A T I O N  R O U T I N E S	*/

/*	The memory allocation routines offered in this file are:

	char *Realloc(ptr, n)	: reallocate buffer to n bytes

	This file imports routines from "system".
*/

#include	<system.h>
#include	"in_all.h"
#include	"alloc.h"

EXPORT char *
Realloc(ptr, sz)
	char ptr[];
	unsigned int sz;
{
	ptr = realloc(ptr, sz);
	if (ptr == 0) No_Mem();
	return ptr;
}
