#include <cpm.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define	OUT_OF_MEMORY (void*)(-1)	/* sbrk returns this on failure */

extern uint8_t _end[1];

int brk(void* newend)
{
	uint8_t* p = newend;
	
	if ((p >= cpm_ramtop) ||
	    (p < _end))	
		return -1;
		
	cpm_ram = (uint8_t*)p;
	return 0;
}

void* sbrk(int increment)
{
	uint8_t* old;
	uint8_t* new;
	
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
