#include "mcg.h"

static struct basicblock* current_bb;

static void rewrite_vregs(struct basicblock* bb,
        int pos, struct vreg* src, struct vreg* dest)
{
    int i, j;

    while (pos < bb->hops.count)
    {
        struct hop* hop = bb->hops.item[pos];

        array_replace(&hop->ins, src, dest);
        array_replace(&hop->throughs, src, dest);
        array_replace(&hop->outs, src, dest);

        for (i=0; i<hop->insels.count; i++)
        {
            struct insel* insel = hop->insels.item[i];
            if ((insel->type == INSEL_VREG) && (insel->u.vreg == src))
                insel->u.vreg = dest;
        }
        
        for (i=0; i<hop->constraints.count; i++)
        {
            struct constraint* c = hop->constraints.item[i].right;

            if (hop->constraints.item[i].left == src)
                hop->constraints.item[i].left = dest;

            if (c->equals_to == src)
                c->equals_to = dest;
        }

        pos++;
    }

    for (i=0; i<bb->nexts.count; i++)
    {
        struct basicblock* nextbb = bb->nexts.item[i];

        for (j=0; j<nextbb->phis.count; j++)
        {
            struct phi* phi = nextbb->phis.item[j].right;
            if (phi->ir->result == src)
                phi->ir->result = dest;
        }
    }
}

static void rewrite_blocks(struct basicblock* startbb, int startindex,
        struct vreg* src, struct vreg* dest)
{
    int i;

    for (i=0; i<dominance.preorder.count; i++)
    {
        struct basicblock* bb = dominance.preorder.item[i];
        if (bb == startbb)
            break;
    }
    assert(i < dominance.preorder.count);

    while (i < dominance.preorder.count)
    {
        struct basicblock* bb = dominance.preorder.item[i];
        rewrite_vregs(bb, startindex, src, dest);
        startindex = 0;
        i++;
    }
}

static int insert_move_after(int index, struct vreg* src)
{
    struct vreg* dest;
    struct hop* copy;

    dest = new_vreg();
    dest->type = src->type;
    copy = new_copy_hop(current_bb, src, dest);

    array_insert(&current_bb->hops, copy, index+1);

    rewrite_blocks(current_bb, index+2, src, dest);
    return 1;
}

void pass_split_live_ranges(void)
{
    int i, j, k;

    for (i=0; i<dominance.preorder.count; i++)
    {
        current_bb = dominance.preorder.item[i];

        for (j=0; j<current_bb->hops.count; j++)
        {
            struct hop* hop = current_bb->hops.item[j];

            if (!hop->is_move)
            {
                for (k=0; k<hop->ins.count; k++)
                {
                    struct vreg* vreg = hop->ins.item[k];
                    j += insert_move_after(j-1, vreg);
                }

                for (k=0; k<hop->outs.count; k++)
                {
                    struct vreg* vreg = hop->outs.item[k];
                    insert_move_after(j, vreg);
                }
            }
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */