/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

#define STACK_BUFFER 128 /* number of bytes to leave for stack */

extern char _end[1];
static char* current = _end;

int brk(void* newend)
{
	/* This variable is used to figure out the current stack pointer,
	 * by taking its address. */
	char dummy;
	char* p = newend;
	
	if ((p > (&dummy - STACK_BUFFER)) ||
	    (p < _end))	
	{
		errno = ENOMEM;
		return -1;
	}
		
	current = p;
	return 0;
}

void* sbrk(int increment)
{
	char* old;
	char* new;

	if (increment == 0)
		return current;
		
	old = current;

	new = old + increment;

	if ((increment > 0) && (new <= old))
		goto out_of_memory;
	else if ((increment < 0) && (new >= old))
		goto out_of_memory;

	if (brk(new) < 0)
		goto out_of_memory;
		
	return old;

out_of_memory:
	errno = ENOMEM;
	return OUT_OF_MEMORY;
}
