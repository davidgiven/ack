#ifndef MEMPOOL_H
#define MEMPOOL_H

struct mempool
{
	struct memnode* top;
	size_t size;
};

extern void mempool_reset(struct mempool* pool);
extern void* mempool_alloc(struct mempool* pool, size_t size);

#endif

