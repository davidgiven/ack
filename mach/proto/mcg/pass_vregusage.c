#include "mcg.h"

static ARRAYOF(struct vreg) vregs;

static void assign_uses_cb(struct hop* hop, void* user)
{
    int i;

    for (i=0; i<hop->ins.count; i++)
        array_appendu(&hop->ins.item[i]->usedhops, hop);

    for (i=0; i<hop->outs.count; i++)
    {
        struct vreg* vreg = hop->outs.item[i];
        assert(vreg->defined == NULL);
        vreg->defined = hop;
        array_appendu(&vregs, vreg);
    }
}

static bool is_spillable_vreg(struct vreg* vreg)
{
    int i;

    if (vreg->defined && !vreg->defined->is_move)
        return false;
    
    for (i=0; i<vreg->usedhops.count; i++)
        if (!vreg->usedhops.item[i]->is_move)
            return false;

    return true;
}

void pass_determine_vreg_usage(void)
{
    int i, j;

    vregs.count = 0;
    hop_walk(assign_uses_cb, NULL);

    for (i=0; i<dominance.preorder.count; i++)
    {
        struct basicblock* bb = dominance.preorder.item[i];
        for (j=0; j<bb->phis.count; j++)
        {
            struct vreg* dest = bb->phis.item[j].left;
            struct phi* phi = bb->phis.item[j].right;
            struct vreg* src = phi->ir->result;
            array_appendu(&src->usedphis, bb);
            array_appendu(&vregs, src);
            array_appendu(&vregs, dest);
        }
    }

    for (i=0; i<vregs.count; i++)
    {
        struct vreg* vreg = vregs.item[i];
        vreg->is_spillable = is_spillable_vreg(vreg);
    }
}

/* vim: set sw=4 ts=4 expandtab : */
