#include "mcg.h"

void pass_convert_copies_to_moves(void)
{
    int i, j;

    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];
        for (j=0; j<bb->hops.count; j++)
        {
            struct hop* hop = bb->hops.item[j];
            if (hop->pseudo && (strcmp(hop->pseudo, "copy") == 0))
            {
                struct valueusage* usage;
                struct hashtable_iterator hit = {};
                struct vreg* invreg = NULL;
                struct vreg* outvreg = NULL;
                while (hashtable_next(hop->valueusage, &hit))
                {
                    usage = hit.value;
                    if (usage->input)
                    {
                        assert(!invreg);
                        invreg = usage->invreg;
                    }
                    if (usage->output)
                    {
                        assert(!outvreg);
                        outvreg = usage->outvreg;
                    }
                }
                assert(invreg && outvreg);

                usage = hop_get_value_usage(hop, outvreg->value);
                assert(!usage->invreg);
                assert(!usage->input);
                usage->input = true;
                usage->invreg = invreg;
                hop->is_move = true;
            }
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */
