/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "libsys.h"

#define	OUT_OF_MEMORY (void*)(-1)	/* sbrk returns this on failure */

static char* current = NULL;
 
int brk(void* end)
{
	int e = _syscall(__NR_brk, (quad) end, 0, 0);
	if (e == -1)
		errno = ENOMEM;
	else
		current = end;
	return e;
}

void* sbrk(int increment)
{
	char* old;
	char* new;
	
	if (!current)
		current = (char*) _syscall(__NR_brk, 0, 0, 0);

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

