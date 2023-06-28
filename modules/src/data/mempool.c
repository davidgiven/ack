#include <stdlib.h>
#include <stdbool.h>
#include "mempool.h"

struct memnode
{
	struct memnode* next;
	char data[0];
};

void mempool_reset(struct mempool* pool)
{
	while (pool->top)
	{
		struct memnode* next = pool->top->next;
		free(pool->top);
		pool->top = next;
	}
}

void* mempool_alloc(struct mempool* pool, size_t size)
{
	struct memnode* node = calloc(1, sizeof(struct memnode) + size);
	node->next = pool->top;
	pool->top = node;
	pool->size += sizeof(struct memnode) + size;
	return &node->data[0];
}

