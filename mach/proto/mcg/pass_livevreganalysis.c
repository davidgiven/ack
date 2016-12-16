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

static bool add_set_to_array(struct set* set, void* arrayp)
{
    bool nochange = true;
    struct array* array = arrayp;
    struct set_iterator sit = {};

    while (set_next(set, &sit))
        nochange &= array_appendu(array, sit.item);

    return nochange;
}

static void add_array_to_set(void* arrayp, struct set* set)
{
    struct array* array = arrayp;
    int i;

    for (i=0; i<array->count; i++)
        set_add(set, array->item[i]);
}

static void remove_array_from_set(void* arrayp, struct set* set)
{
    struct array* array = arrayp;
    int i;

    for (i=0; i<array->count; i++)
        set_remove(set, array->item[i]);
}

static void propagate_liveness(struct basicblock* bb)
{
    static struct set current;
	int i, j;

    set_empty(&current);

    add_array_to_set(&bb->liveouts, &current);

	for (i=bb->hops.count-1; i>=0; i--)
	{
		struct hop* hop = bb->hops.item[i];

        remove_array_from_set(&hop->outs, &current);
        finished &= add_set_to_array(&current, &hop->throughs);
        add_array_to_set(&hop->ins, &current);
	}

    for (i=0; i<bb->phis.count; i++)
        set_remove(&current, bb->phis.item[i].left);

    finished &= add_set_to_array(&current, &bb->liveins);

	for (i=0; i<bb->prevs.count; i++)
	{
		struct basicblock* prev = bb->prevs.item[i];
        finished &= add_set_to_array(&current, &prev->liveouts);
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
}

/* vim: set sw=4 ts=4 expandtab : */

