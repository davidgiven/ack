#include "mcg.h"

static struct basicblock* current_bb;

static void calculate_spillibility(void)
{
    int i;

    for (i=0; i<current_bb->hops.count; i++)
    {
        struct hop* hop = current_bb->hops.item[i];
        if (!hop->is_move)
        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(hop->valueusage, &hit))
            {
                struct value* value = hit.key;
                struct valueusage* usage = hit.value;

                if (!usage->through)
                {
                    if (usage->invreg)
                        usage->invreg->needs_register = true;
                    if (usage->outvreg)
                        usage->outvreg->needs_register = true;
                }
            }
        }
    }
}

void pass_calculate_vreg_spillibility(void)
{
    int i;

    for (i=0; i<dominance.preorder.count; i++)
    {
        current_bb = dominance.preorder.item[i];
        calculate_spillibility();
    }
}

/* vim: set sw=4 ts=4 expandtab : */
