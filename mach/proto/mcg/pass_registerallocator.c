#include "mcg.h"

static ARRAYOF(struct hreg) hregs;
static int stacksize;

static void populate_hregs(void)
{
    const struct burm_register_data* brd = burm_register_data;

    stacksize = 0;
    while (brd->name)
    {
        array_append(&hregs, new_hreg(brd));
        brd++;
    }
}

static void wire_up_blocks_ins_outs(void)
{
    int i, j;

    for (i=0; i<dominance.preorder.count; i++)
    {
        struct basicblock* bb = dominance.preorder.item[i];
        assert(bb->hops.count >= 1);
        bb->regsin = &bb->hops.item[0]->regsin;
        bb->regsout = &bb->hops.item[bb->hops.count-1]->regsout;
    }
}

static struct hreg* allocate_hreg(register_assignment_t* regs, struct vreg* vreg)
{
    int i;

    for (i=0; i<hregs.count; i++)
    {
        struct hreg* hreg = hregs.item[i];
        if (!pmap_findleft(regs, hreg))
        {
            pmap_put(regs, hreg, vreg);
            return hreg;
        }
    }

    fatal("ran out of registers");
}

static void select_registers(struct hop* hop,
    register_assignment_t* old, register_assignment_t* in, register_assignment_t* out)
{
    int i;

    /* First, any vregs passing through the instruction stay in the same
     * registers they are currently in. */

    for (i=0; i<hop->throughs.count; i++)
    {
        struct vreg* vreg = hop->throughs.item[i];
        struct hreg* hreg = pmap_findright(old, vreg);
        assert(hreg != NULL);

        pmap_put(in, hreg, vreg);
        pmap_put(out, hreg, vreg);
    }

    /* Any registers being *read* by the instruction should also stay where
     * they are. (This is likely to duplicate some throughs.) */

    for (i=0; i<hop->ins.count; i++)
    {
        struct vreg* vreg = hop->ins.item[i];
        struct hreg* hreg = pmap_findright(old, vreg);
        assert(hreg != NULL);

        pmap_put(in, hreg, vreg);
    }

    /* Any output registers will be *new* vregs (because SSA). So, allocate
     * new hregs for them. */

    for (i=0; i<hop->outs.count; i++)
    {
        struct vreg* vreg = hop->outs.item[i];
        allocate_hreg(out, vreg);
    }
}

void pass_register_allocator(void)
{
    int i, j, k;

    populate_hregs();
    wire_up_blocks_ins_outs();

    for (i=0; i<dominance.preorder.count; i++)
    {
        struct basicblock* bb = dominance.preorder.item[i];
        register_assignment_t* old = bb->regsin;
        
        tracef('R', "R: allocating block %s\n", bb->name);

        /* Attempt to import any block input registers from a predecessor. At
         * least one predecessor should export it; our graph traversal order
         * guarantees it. */

        for (j=0; j<bb->liveins.count; j++)
        {
            struct vreg* vreg = bb->liveins.item[j];
            for (k=0; k<bb->prevs.count; k++)
            {
                struct basicblock* prevbb = bb->prevs.item[k];
                struct hreg* hreg = pmap_findright(prevbb->regsout, vreg);
                if (hreg)
                {
                    tracef('R', "R: import hreg %s for input %%%d from %s\n",
                        hreg->name, vreg->id, prevbb->name);
                    assert(!pmap_findleft(old, hreg));
                    pmap_put(old, hreg, vreg);
                    goto nextvreg;
                }
            }

            fatal("couldn't find a register assignment for $%d", vreg->id);
            nextvreg:;
        }

        /* And now do the same for the phis. Unlike input vregs, a phi can
         * import a vreg from multiple locations. However, we don't care which
         * one we find, as this is just a hint. Picking the same register as a
         * predecessor helps reduce the number of copies the SSA deconstruction
         * pass will need to insert. */

        for (j=0; j<bb->phis.count; j++)
        {
            struct vreg* vreg = bb->phis.item[j].left;
            struct phi* phi = bb->phis.item[j].right;
            if (!pmap_findright(old, vreg))
            {
                /* This variable isn't allocated yet. */

                struct hreg* hreg = pmap_findright(
                    phi->prev->regsout, phi->ir->result);
                if (hreg && !pmap_findleft(old, hreg))
                {
                    tracef('R', "R: import hreg %s for phi input %%%d from %s\n",
                        hreg->name, vreg->id, phi->prev->name);
                    pmap_put(old, hreg, vreg);
                }
            }
        }

        /* It's possible for the previous stage to fail because in in has
         * clobbered the physical register we were wanting. So we need to
         * allocate a new register for that phi value. */

        for (j=0; j<bb->phis.count; j++)
        {
            struct vreg* vreg = bb->phis.item[j].left;
            if (!pmap_findright(old, vreg))
                allocate_hreg(old, vreg);
        }
            
        for (j=0; j<bb->hops.count; j++)
        {
            struct hop* hop = bb->hops.item[j];
            register_assignment_t* in = &hop->regsin;
            register_assignment_t* out = &hop->regsout;;

            select_registers(hop, old, in, out);

            old = out;
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */

