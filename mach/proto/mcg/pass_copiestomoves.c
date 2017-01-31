#include "mcg.h"

void pass_convert_copies_to_moves(void)
{
    int i, j, k;

    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];
        for (j=0; j<bb->hops.count; j++)
        {
            struct hop* hop = bb->hops.item[j];
            if (hop->is_move)
            {
                struct vreg* invreg = NULL;
                struct vreg* outvreg = NULL;

                for (k=0; k<hop->vregusage.count; k++)
                {
                    struct vreg* in = hop->vregusage.item[k].left;
                    struct vreg* out = hop->vregusage.item[k].right;
                    static struct valueusage dummyusage = {};
                    struct valueusage* inusage = in ? hop_get_value_usage(hop, in->value) : &dummyusage;
                    struct valueusage* outusage = out ? hop_get_value_usage(hop, out->value) : &dummyusage;

                    if (inusage->input && !inusage->output)
                    {
                        assert(!invreg);
                        invreg = in;
                    }
                    if (!inusage->input && outusage->output)
                    {
                        assert(!outvreg);
                        outvreg = out;
                    }
                }

                if (invreg && outvreg)
                    hop_add_through_vreg(hop, invreg, outvreg);
            }
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */
