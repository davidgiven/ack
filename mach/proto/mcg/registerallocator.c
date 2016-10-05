#include "mcg.h"

static ARRAYOF(struct basicblock) blocks;

static void recursively_walk_blocks(struct basicblock* bb)
{
	int i;

	if (array_appendu(&blocks, bb))
		return;
	tracef('R', "R: considering block %s\n", bb->name);

	for (i=0; i<bb->nexts.count; i++)
		recursively_walk_blocks(bb->nexts.item[i]);
}

static void order_blocks(struct procedure* proc)
{
	/* Put them into preorder; this ensures that when we do the allocation,
	 * we do all of a block's predecessors before the block (except for
	 * backward edges). */
	
	blocks.count = 0;
	recursively_walk_blocks(proc->blocks.item[0]);
	assert(blocks.count == proc->blocks.count);
}

void register_allocator(struct procedure* proc)
{
	order_blocks(proc);
}

/* vim: set sw=4 ts=4 expandtab : */

