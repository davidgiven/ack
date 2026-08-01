#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <emu6800.h>

#define	OUT_OF_MEMORY (void*)(-1)	/* sbrk returns this on failure */

extern uint8_t _end[1];

int brk(void* newend)
{
	uint8_t* p = newend;
	
	if ((p >= emu6800_ramtop) ||
	    (p < _end))	{
		return -1;
	}
		
	emu6800_ram = (uint8_t*)p;
	return 0;
}

void* sbrk(int increment)
{
	uint8_t* old;
	uint8_t* new;

	if (increment == 0)
		return emu6800_ram;
		
	old = emu6800_ram;
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
