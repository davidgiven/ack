/* This is an ANSI C version of the classic K&R memory allocator, with
 * some improvements stolen from the Fuzix libc.
 */

#ifndef MALLOC_H
#define MALLOC_H

typedef struct block_s {
	struct block_s* next;
	size_t size; /* in sizeof(memblock_t) units */
} memblock_t;

extern memblock_t __mem_root;
extern memblock_t* __mem_first_free;

#define BLOCKOF(p) (((memblock_t*)(p)) - 1)

/* Smallest amount to allocate from brk */
#define BRKSIZE (512 / sizeof(memblock_t))

#define BLOCKCOUNT(bytes) \
	(bytes + sizeof(memblock_t) + sizeof(memblock_t) - 1)

#endif

