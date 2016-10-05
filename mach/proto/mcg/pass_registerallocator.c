#include "mcg.h"

static ARRAYOF(struct basicblock) blocks;

static void recursively_walk_dominance_graph(struct basicblock* bb)
{
	int i;

	array_append(&blocks, bb);
	tracef('R', "R: considering block %s\n", bb->name);

	/* Skip the entry block (which is its own dominator). */
	
	for (i=1; i<dominators.count; i++)
	{
		struct basicblock* left = dominators.item[i].left;
		struct basicblock* right = dominators.item[i].right;
		if (right == bb)
			recursively_walk_dominance_graph(left);
	}
}

void pass_register_allocator(struct procedure* proc)
{
	calculate_dominance_graph(proc);

	blocks.count = 0;
	recursively_walk_dominance_graph(proc->blocks.item[0]);
	assert(blocks.count == proc->blocks.count);
}

/* vim: set sw=4 ts=4 expandtab : */

