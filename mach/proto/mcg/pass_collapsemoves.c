#include "mcg.h"

static struct vreg* find_source_of_vreg(struct hop* hop, struct vreg* vreg);

static void merge(struct hop* master, struct hop* slave)
{
    int i, j;

    for (i=0; i<slave->vregusage.count; i++)
    {
        struct vreg* sinput = slave->vregusage.item[i].left;
        struct vreg* soutput = slave->vregusage.item[i].right;

        if (sinput && soutput)
        {
            for (j=0; j<master->vregusage.count; j++)
            {
                struct vreg* minput = master->vregusage.item[j].left;
                struct vreg* moutput = master->vregusage.item[j].right;

                if (minput && moutput && (sinput == moutput))
                {
                    /* The input of the slave was written by the master; so instead source
                     * the slave's copy from the source of the master. */
                    sinput = minput;
                    break;
                }
            }

            hop_add_through_vreg(master, sinput, soutput);
        }
    }
}

void pass_collapse_adjacent_moves(void)
{
    int i, j;

    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];

        j = 0;
        while (j < (bb->hops.count-1))
        {
            struct hop* firstmove = bb->hops.item[j];
            struct hop* secondmove = bb->hops.item[j+1];
            if (firstmove->is_move && secondmove->is_move)
            {
                merge(firstmove, secondmove);
                array_remove(&bb->hops, secondmove);
            }
            else
                j++;
        }
    }
}
