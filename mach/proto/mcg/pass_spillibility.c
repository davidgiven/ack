#include "mcg.h"

static struct basicblock* current_bb;

static void calculate_spillibility(void)
{
    int i, j;

    for (i=0; i<current_bb->hops.count; i++)
    {
        struct hop* hop = current_bb->hops.item[i];
        if (!hop->is_move)
        {
            for (j=0; j<hop->vregusage.count; j++)
            {
                struct vreg* invreg = hop->vregusage.item[j].left;
                struct vreg* outvreg = hop->vregusage.item[j].right;
                struct valueusage* usage = invreg ? hop_get_value_usage(hop, invreg->value) : NULL;

                if (invreg && !outvreg)
                    invreg->needs_register = true;
                if (!invreg && outvreg)
                    outvreg->needs_register = true;
                if (invreg && outvreg && !usage->through)
                    invreg->needs_register = outvreg->needs_register = true;
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
