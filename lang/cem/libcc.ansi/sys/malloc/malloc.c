#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "malloc.h"

#if ACKCONF_WANT_MALLOC

block_t __mem_root = { &__mem_root, 0 };
block_t* __mem_freelist = &__mem_root;

/* Pulls more memory from the system. */

static block_t* brkmore(size_t nb)
{
	uintptr_t bytes;
	block_t* p;

	if (nb < BRKSIZE)
		nb = BRKSIZE;
	bytes = nb * sizeof(block_t);

	/* Danger, will robinson! sbrk's parameter is *signed*... but malloc() takes a
	 * size_t. */

	if (bytes > INTPTR_MAX)
		return NULL;

	p = sbrk(bytes);
	if (p == (block_t*)-1)
		return NULL;

	/* Add it to the free list by pretending it's a used block and freeing it. */

	p->size = nb;
	free(p + 1);
	return __mem_freelist;
}

void* malloc(size_t size)
{
	block_t* p;
	block_t* prev;
	size_t nblocks;

	/* Add on space for the header; make sure we allocate a round number
	 * of blocks; avoid overflow. */
	nblocks = BLOCKCOUNT(size);
	if (nblocks < size)
		return NULL;
	nblocks /= sizeof(block_t);

	prev = __mem_freelist;
	p = prev->next;
	for (;;)
	{
		if (p->size == nblocks)
		{
			/* We found a hole of exactly the right size. Unlink and return it.
			 * The size field is already set. */
			prev->next = p->next;
			__mem_freelist = prev;
			return (void*)(p + 1);
		}
		else if (p->size > nblocks)
		{
			/* We found a hole bigger than we need. We shrink the hole and return
			 * what's left. */
			p->size -= nblocks;
			p += p->size; /* p now points at our new block */
			p->size = nblocks;
			__mem_freelist = prev;
			return (void*)(p + 1);
		}

		if (p == __mem_freelist)
		{
			/* Uh-oh --- we've gone right round the ring and haven't found
			 * anything. Get more memory from the system and keep going. */
			p = brkmore(nblocks);
			if (!p)
				return NULL;
		}

		prev = p;
		p = p->next;
	}
}

void free(void* ptr)
{
	block_t* h = BLOCKOF(ptr);
	block_t* p;

	if (!ptr)
		return;

	/* __mem_freelist points into an ordered ring of free blocks. First,
	 * we run around the ring until we find the last block before this one.
	 */

	p = __mem_freelist;
	for (;;)
	{
		/* Is h between p and the block after p? If so, h needs to be inserted
		 * after p, so stop here. */
		if ((p < h) && (h < p->next))
			break;

		/* Is p the last block before the end of the address space? */
		if (p >= p->next)
		{
			/* Is h after p? (That is, will it become the new last block?) */
			if (p < h)
				break;

			/* Is h going to become the new *first* block? */
			if (h < p->next)
				break;
		}

		p = p->next;
	}

	/* If we can, merge the next block onto the end of h. */

	if ((h + h->size) == p->next)
	{
		h->size += p->next->size;
		h->next = p->next->next;
	}
	else
	{
		/* Otherwise, insert h before p->next. */
		h->next = p->next;
	}

	/* Now try to merge h onto the end of p. */

	if ((p + p->size) == h)
	{
		p->size += h->size;
		p->next = h->next;
	}
	else
	{
		/* Okay, we couldn't do the merge. Fix up the linked list. */
		p->next = h;
	}

	/* ...and update the ring pointer. */
	__mem_freelist = p;
}

#endif
