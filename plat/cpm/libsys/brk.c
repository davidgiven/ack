/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define	OUT_OF_MEMORY (void*)(-1)	/* sbrk returns this on failure */
#define STACK_BUFFER 128 /* number of bytes to leave for stack */

extern char _end[1];
static char* current = _end;

int brk(void* newend)
{
	/* We determine the amount of free memory by looking at the address of the
	 * BDOS vector at 0x0006. */
	char* memtop = (char*) ((*(unsigned char*)0x0007)<<8);
	char* p = newend;
	
	if ((p >= memtop) ||
	    (p < _end))	
		return -1;
		
	current = p;
	return 0;
}

void* sbrk(intptr_t increment)
{
	char* old;
	
	if (increment == 0)
		return current;
		
	old = current;
	if (brk(old + increment) < 0)
		return OUT_OF_MEMORY;
		
	return old;
}
