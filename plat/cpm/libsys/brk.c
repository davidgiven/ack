#include <cpm.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define	OUT_OF_MEMORY (void*)(-1)	/* sbrk returns this on failure */
#define STACK_BUFFER 128 /* number of bytes to leave for stack */

extern char _end[1];

int brk(void* newend)
{
	/* We determine the amount of free memory by looking at the address of the
	 * BDOS vector at 0x0006. */
	char* memtop = (char*) ((*(unsigned char*)0x0007)<<8);
	char* p = newend;
	
	if ((p >= memtop) ||
	    (p < _end))	
		return -1;
		
	cpm_ram = p;
	return 0;
}

void* sbrk(int increment)
{
	char* old;
	char* new;
	
	if (increment == 0)
		return cpm_ram;
		
	old = cpm_ram;
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
