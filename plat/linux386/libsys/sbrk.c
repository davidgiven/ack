/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <unistd.h>
#include "libsys.h"

#define	OUT_OF_MEMORY (void*)(-1)	/* sbrk returns this on failure */

static char* current = NULL;
 
void* sbrk(intptr_t increment)
{
	char* old;
	char* new;
	char* actual;
	
	if (!current)
		current = (char*) _syscall(__NR_brk, 0, 0, 0);

	if (increment == 0)
		return current;
		
	old = current;
	new = old + increment;

	actual = (char*) _syscall(__NR_brk, (quad) new, 0, 0);
	if (actual < new)
		return OUT_OF_MEMORY;
		
	current = actual;
	return old;
}
