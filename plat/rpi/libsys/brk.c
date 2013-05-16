/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
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
	/* This variable is used to figure out the current stack pointer,
	 * by taking its address. */
	char dummy;
	char* p = newend;
	
	if ((p > (&dummy - STACK_BUFFER)) ||
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
