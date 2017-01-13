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

        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(hop->valueusage, &hit))
            {
                struct value* value = hit.key;
                struct valueusage* usage = hit.value;

                if (usage->output)
                    set_remove(&known_live, value);
            }
        }

        /* Propagate the set of live values across this hop. */

        {
            struct set_iterator si = {};
            while (set_next(&known_live, &si))
            {
                struct value* value = si.item;
                hop_get_value_usage(hop, value)->through = true;
            }
        }

        /* Values which are read from must have come from somewhere, and so
         * become live. */

        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(hop->valueusage, &hit))
            {
                struct value* value = hit.key;
                struct valueusage* usage = hit.value;

                if (usage->input)
                    set_remove(&known_live, value);
            }
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

