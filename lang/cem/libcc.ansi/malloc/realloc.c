#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "malloc.h"

void* realloc(void* ptr, size_t size)
{
	block_t* h;
	size_t nblocks;
	void* newptr;

	if (size == 0)
	{
		free(ptr);
		return NULL;
	}

	if (!ptr)
		return malloc(size);

	h = BLOCKOF(ptr);

	nblocks = BLOCKCOUNT(size);
	/* Overflow check. */
	if (nblocks < size)
		return NULL;

	/* Shrinking the block? Don't bother doing anything (it's never worth it). */
	if (nblocks <= h->size)
		return ptr;

	/* Allocate and copy. */

	newptr = malloc(size);
	if (!newptr)
		return NULL;
	memcpy(newptr, ptr, h->size * sizeof(block_t));
	free(ptr);
	return newptr;
}
