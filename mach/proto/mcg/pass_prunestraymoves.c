#include "mcg.h"

static struct vreg* find_source_of_vreg(struct hop* hop, struct vreg* vreg);

static void prune(struct hop* master, struct hop* slave)
{
    int i, j;

    for (i=0; i<master->vregusage.count; i++)
    {
        struct vreg* minput = master->vregusage.item[i].left;
        struct vreg* moutput = master->vregusage.item[i].right;

        if (moutput)
        {
            bool found = false;

            for (j=0; j<slave->vregusage.count; j++)
            {
                struct vreg* sinput = slave->vregusage.item[j].left;
                struct vreg* soutput = slave->vregusage.item[j].right;

                if (sinput == moutput)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                pmap_remove_index(&master->vregusage, i);
                i--;
            }
        }
    }
}

void pass_prune_stray_moves(void)
{
    int i, j;

    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];

        for (j=0; j<(bb->hops.count-1); j++)
        {
            struct hop* firstmove = bb->hops.item[j];
            struct hop* secondmove = bb->hops.item[j+1];
            if (firstmove->is_move && !secondmove->is_move)
                prune(firstmove, secondmove);
        }
    }
}

