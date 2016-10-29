#include "mcg.h"

static bool finished;

static void preload_blocks(void)
{
    /* Any variable referenced in a phi *must* be a liveout of one of our
     * predecessors. */

    int i, j;
    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];

        for (j=0; j<bb->phis.count; j++)
        {
            struct vreg* vreg = bb->phis.item[j].left;
            struct phi* phi = bb->phis.item[j].right;

            assert(array_contains(&bb->prevs, phi->prev));
            array_appendu(&phi->prev->liveouts, phi->ir->result);
        }
    }
}

static void propagate_liveness(struct basicblock* bb)
{
	static ARRAYOF(struct vreg) current;
	int i;

	current.count = 0;
	array_appendall(&current, &bb->liveouts);

	for (i=bb->hops.count-1; i>=0; i--)
	{
		struct hop* hop = bb->hops.item[i];

		array_removeall(&current, &hop->outs);
		finished &= array_appendallu(&hop->throughs, &current);
		array_appendallu(&current, &hop->ins);
	}

    for (i=0; i<bb->phis.count; i++)
        array_remove(&current, bb->phis.item[i].left);

	finished &= array_appendallu(&bb->liveins, &current);

	for (i=0; i<bb->prevs.count; i++)
	{
		struct basicblock* prev = bb->prevs.item[i];
		finished &= array_appendallu(&prev->liveouts, &current);
	}
}

void pass_live_vreg_analysis(void)
{
	int i;

    preload_blocks();

	do
	{
		finished = true;

		tracef('L', "L: beginning liveness pass\n");
		for (i=0; i<dominance.postorder.count; i++)
			propagate_liveness(dominance.postorder.item[i]);
	}
	while (!finished);

    //assert(cfg.entry->liveins.count == 0);
}

/* vim: set sw=4 ts=4 expandtab : */

