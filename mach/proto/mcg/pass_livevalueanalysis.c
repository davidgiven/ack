#include "mcg.h"

static struct set known_live = { HASHTABLE_OF_VALUES };

static void propagate_liveness(struct basicblock* bb)
{
    static struct set current;
	int i, j;

    set_empty(&known_live);

    for (i=0; i<bb->exports.count; i++)
        set_add(&known_live, bb->exports.item[i].left);

	for (i=bb->hops.count-1; i>=0; i--)
	{
		struct hop* hop = bb->hops.item[i];

        /* Values are only ever written once, so if we see a write then we
         * know the value was not live before this. */

        for (j=0; j<hop->outputs.count; j++)
        {
            struct value* value = hop->outputs.item[j];
            set_remove(&known_live, value);
        }

        /* Propagate the set of live values across this hop. */

        {
            struct set_iterator si = {};
            while (set_next(&known_live, &si))
            {
                struct value* value = si.item;
                array_appendu(&hop->throughs, value);
            }
        }

        /* Values which are read from must have come from somewhere, and so
         * become live. */

        for (j=0; j<hop->inputs.count; j++)
        {
            struct value* value = hop->inputs.item[j];
            set_add(&known_live, value);
        }
	}
}

void pass_live_value_analysis(void)
{
	int i;

    for (i=0; i<dominance.postorder.count; i++)
        propagate_liveness(dominance.postorder.item[i]);
}

/* vim: set sw=4 ts=4 expandtab : */

