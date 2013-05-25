/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pi.h>

#define	OUT_OF_MEMORY (void*)(-1)	/* sbrk returns this on failure */
#define STACK_BUFFER 1024 /* number of bytes to leave for stack */

extern char _end[1];
static char* current = _end;

/* Top of heap: we assume that the block of memory the binary is loaded in
 * is 256kB long. Because user pointers are always relative to the beginning
 * of the block, this makes the end address easy to calculate. */
static char* max = (char*) (256*1024);

int brk(void* newend)
{
	if ((newend >= (void*)max) || (newend < (void*)_end))
		return -1;

	current = newend;
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
