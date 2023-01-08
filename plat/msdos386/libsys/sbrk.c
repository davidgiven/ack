/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

extern char _end[1];
static char* current = _end;

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
		
	current = new;
	return old;

out_of_memory:
	errno = ENOMEM;
	return OUT_OF_MEMORY;
}
