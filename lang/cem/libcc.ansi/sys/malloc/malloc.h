/* This is an ANSI C version of the classic K&R memory allocator, with
 * some improvements stolen from the Fuzix libc.
 */

#ifndef MALLOC_H
#define MALLOC_H

typedef struct block_s {
	struct block_s* next;
	size_t size; /* in sizeof(block_t) units */
} block_t;

extern block_t __mem_root;
extern block_t* __mem_first_free;

#define BLOCKOF(p) (((block_t*)(p)) - 1)

/* Smallest amount to allocate from brk */
#define BRKSIZE (512 / sizeof(block_t))

#define BLOCKCOUNT(bytes) \
	(bytes + sizeof(block_t) + sizeof(block_t) - 1)

#endif

