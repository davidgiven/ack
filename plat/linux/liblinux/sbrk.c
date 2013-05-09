/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/sbrk.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "libsys.h"

#define	OUT_OF_MEMORY (void*)(-1)	/* sbrk returns this on failure */

extern char _end[1];

static char* current = _end;
 
void* sbrk(intptr_t increment)
{
	char* old;
	char* new;

	if (increment == 0)
		return current;
		
	old = current;
	new = old + increment;
	if (brk(new) < 0)
		return OUT_OF_MEMORY;
		
	current = new;
	return old;
}
