#include "mcg.h"

struct assignment
{
    struct vreg* in;
    struct vreg* out;
};

static struct procedure* current_proc;

static ARRAYOF(struct hreg) hregs;

static PMAPOF(struct vreg, struct hreg) evicted;
static struct hop* current_hop;
static register_assignment_t* current_ins;
static register_assignment_t* current_outs;

static int insert_moves(struct basicblock* bb, int index,
    register_assignment_t* srcregs, register_assignment_t* destregs);

static void populate_hregs(void)
{
    const struct burm_register_data* brd = burm_register_data;

    hregs.count = 0;
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

static struct hreg* allocate_phi_hreg(register_assignment_t* regs,
    struct vreg* vreg, uint32_t type)
{
    int i;

    /* We need a new register at the beginning of the block to put a phi value
     * into. */

    for (i=0; i<hregs.count; i++)
    {
        struct hreg* hreg = hregs.item[i];
        if (!pmap_findleft(regs, hreg) && (hreg->type == type))
        {
            /* This one is unused. Use it. */
            return hreg;
        }
    }

    /* We'll need to allocate a new stack slot for it. */
    assert(false);
}

static bool evictable(struct hreg* hreg, struct vreg* vreg)
{
    struct constraint* c = pmap_findleft(&current_hop->constraints, vreg);
    return (hreg->attrs & c->attrs) && !array_contains(&current_hop->ins, vreg);
    /* Find an unused output register of the right class which is not also being used as an input register. */
}

static struct hreg* evict(struct vreg* vreg)
{
    int i;

    /* Look for a through register which matches our requirements. We should be
     * doing some calculation here to figure out the cheapest register to
     * evict, but for now we're picking the first one. FIXME. */

    for (i=0; i<hregs.count; i++)
    {
        struct hreg* hreg = hregs.item[i];
        struct vreg* candidate = pmap_findleft(current_ins, hreg);

        if (candidate &&
            (pmap_findleft(current_outs, hreg) == candidate) &&
            evictable(hreg, vreg))
        {
            tracef('R', "R: evicting %%%d from %s\n", candidate->id, hreg->name);
            pmap_put(&evicted, candidate, hreg);
            pmap_remove(current_ins, hreg, candidate);
            pmap_remove(current_outs, hreg, candidate);
            return hreg;
        }
    }

    /* Couldn't find anything to evict */
    assert(false);
}

static bool type_match(struct hreg* hreg, struct vreg* vreg)
{
    struct constraint* c = pmap_findleft(&current_hop->constraints, vreg);
    if (c)
        return (hreg->attrs & c->attrs);
    if (vreg->congruence)
        return (hreg->type == vreg->congruence->type);
    return (hreg->type == vreg->type);
}

static bool allocatable_stackable_input(struct hreg* hreg, struct vreg* vreg)
{
    return !pmap_findleft(current_ins, hreg) &&
        type_match(hreg, vreg);
}

static bool allocatable_stackable_output(struct hreg* hreg, struct vreg* vreg)
{
    return !pmap_findleft(current_outs, hreg) &&
        type_match(hreg, vreg) &&
        !(hreg->attrs & current_hop->insndata->corrupts);
}

static bool allocatable_input(struct hreg* hreg, struct vreg* vreg)
{
    return allocatable_stackable_input(hreg, vreg) &&
        !hreg->is_stacked;
}

static bool allocatable_output(struct hreg* hreg, struct vreg* vreg)
{
    return allocatable_stackable_output(hreg, vreg) &&
        !hreg->is_stacked;
}

static struct hreg* find_input_reg(struct vreg* vreg)
{
    int i;
    struct hreg* hreg = NULL;

    for (i=0; i<hregs.count; i++)
    {
        hreg = hregs.item[i];
        if (allocatable_input(hreg, vreg))
        {
            return hreg;
        }
    }

    return NULL;
}

static struct hreg* find_output_reg(struct vreg* vreg)
{
    int i;
    struct hreg* hreg = NULL;

    for (i=0; i<hregs.count; i++)
    {
        hreg = hregs.item[i];
        if (allocatable_output(hreg, vreg))
        {
            return hreg;
        }
    }

    return NULL;
}

static struct hreg* find_through_reg(struct vreg* vreg)
{
    int i;
    struct hreg* hreg = NULL;

    for (i=0; i<hregs.count; i++)
    {
        hreg = hregs.item[i];
        if (allocatable_input(hreg, vreg) &&
            allocatable_output(hreg, vreg))
        {
            return hreg;
        }
    }

    return NULL;
}

static void add_input_register(struct vreg* vreg, struct hreg* hreg)
{
    int i;

    /* Register hint for an input? */

    if (hreg)
    {
        if (hreg->is_stacked)
        {
            /* This vreg is stacked; we need to put it in a register. That's
             * slightly exciting because the vreg might be a through, which
             * means we need an output register too... which we might not be
             * able to allocate. */

            if (array_contains(&current_hop->throughs, vreg))
            {
                struct hreg* src = hreg;
                hreg = find_through_reg(vreg);
                assert(hreg);
                pmap_remove(current_ins, src, vreg);
                pmap_remove(current_outs, src, vreg);
                pmap_add(current_ins, hreg, vreg);
                pmap_add(current_outs, hreg, vreg);
                return;
            }
            else
            {
                /* Not a through. */
                pmap_remove(current_ins, hreg, vreg);
                hreg = NULL;
            }
        }
        else if (pmap_findleft(current_ins, hreg) == vreg)
        {
            /* Yup, already there. */
        }
        else if (allocatable_input(hreg, vreg))
        {
            /* The register is free. */
        }
        else
        {
            /* Can't honour the hint. */
            hreg = NULL;
        }
    }

    if (!hreg)
    {
        /* Find an unused input register of the right class. */

        hreg = find_input_reg(vreg);
        if (!hreg)
            hreg = evict(vreg);
    }

    pmap_add(current_ins, hreg, vreg);
}

static void add_output_register(struct vreg* vreg)
{
    struct hreg* hreg;
    int i;
    struct constraint* c;

    /* Is this register supposed to be the same as one of the input registers?
     * */

    c = pmap_findleft(&current_hop->constraints, vreg);
    if (c->equals_to)
    {
        tracef('R', "R: output equality constraint of %%%d to %%%d\n",
            vreg->id, c->equals_to->id);

        /* This output register is constrained to be in the same hreg as an
         * input register (most likely for a 2op instruction). */

        hreg = pmap_findright(current_ins, c->equals_to);

        /* If this register is currently unused as an output, use it. */

        if (allocatable_output(hreg, c->equals_to))
        {
            pmap_add(current_outs, hreg, vreg);
            return;
        }

        /* Okay, something's in it. Most likely it's a through being used as an
         * input register.  Trying to evict it would be pointless as that would
         * also evict the input. So, we're going to have to do this the hard
         * way: we try to allocate a matched set of input and output registers.
         * */

        hreg = find_through_reg(vreg);
        if (!hreg)
            hreg = evict(vreg);

        pmap_add(current_outs, hreg, vreg);
        tracef('R', "R: output equality constraint requires extra move of %%%d => %s\n",
            c->equals_to->id, hreg->name);
        pmap_add(current_ins, hreg, c->equals_to);
    }
    else
    {
        /* This is an ordinary new register. */

        hreg = find_output_reg(vreg);
        if (!hreg)
            hreg = evict(vreg);

        pmap_add(current_outs, hreg, vreg);
    }
}

static void add_through_register(struct vreg* vreg, struct hreg* hreg)
{
    /* Register hint for an input? */

    if (hreg)
    {
        bool infree = allocatable_stackable_input(hreg, vreg);
        bool outfree = allocatable_stackable_output(hreg, vreg);
        struct vreg* inuse = pmap_findleft(current_ins, hreg);
        struct vreg* outuse = pmap_findleft(current_outs, hreg);

        if ((infree || (inuse == vreg)) &&
            (outfree || (outuse == vreg)))
        {
            /* Input and output are either free or already assigned to this
             * vreg. */
        }
        else
        {
            /* Nope, can't honour the hint. Mark the register as evicted; we'll
             * put it in something later (probably a stack slot). */

            tracef('R', "R: cannot place %%%d in %s, evicting\n", vreg->id, hreg->name);
            pmap_put(&evicted, vreg, hreg);
            pmap_remove(current_ins, hreg, vreg);
            pmap_remove(current_outs, hreg, vreg);
            return;
        }
    }
    
    assert(hreg);
    pmap_put(current_ins, hreg, vreg);
    pmap_put(current_outs, hreg, vreg);
}

static void find_new_home_for_evicted_register(struct vreg* vreg, struct hreg* src)
{
    uint32_t srctype = src->type;
    struct hreg* hreg;
    int i;

    /* Find an unused output register of the right class which is not also
     * being used as an input register. */

    hreg = NULL;
    for (i=0; i<hregs.count; i++)
    {
        hreg = hregs.item[i];
        if ((hreg->type == src->type) &&
            allocatable_stackable_input(hreg, vreg) &&
            allocatable_stackable_output(hreg, vreg))
        {
            goto found;
        }
    }

    /* No more registers --- allocate a stack slot. */

    hreg = new_stacked_hreg(src->type);
    array_append(&hregs, hreg);

found:
    tracef('R', "R: evicted %%%d moving to %s\n", vreg->id, hreg->name);
    pmap_add(current_ins, hreg, vreg);
    pmap_add(current_outs, hreg, vreg);
}

static void select_registers(struct hop* hop,
    register_assignment_t* old, register_assignment_t* in, register_assignment_t* out)
{
    int i;

    current_hop = hop;
    current_ins = in;
    current_outs = out;
    evicted.count = 0;

    /* First, any vregs passing through the instruction stay in the same
     * registers they are currently in. */

    for (i=0; i<hop->throughs.count; i++)
    {
        struct vreg* vreg = hop->throughs.item[i];
        struct hreg* hreg = pmap_findright(old, vreg);
        add_through_register(vreg, hreg);
    }

    /* Any registers being *read* by the instruction should also stay where
     * they are. (This is likely to duplicate some throughs.) */

    for (i=0; i<hop->ins.count; i++)
    {
        struct vreg* vreg = hop->ins.item[i];
        struct hreg* hreg = pmap_findright(old, vreg);
        add_input_register(vreg, hreg);
    }

    /* Any output registers will be *new* vregs (because SSA). So, allocate
     * new hregs for them. */

    for (i=0; i<hop->outs.count; i++)
    {
        struct vreg* vreg = hop->outs.item[i];
        add_output_register(vreg);
    }

    /* Any evicted registers now need to go somewhere (potentially, the stack).
     * */

    for (i=0; i<evicted.count; i++)
    {
        struct vreg* vreg = evicted.item[i].left;
        struct hreg* src = evicted.item[i].right;
        find_new_home_for_evicted_register(vreg, src);
    }
}

static void assign_hregs_to_vregs(void)
{
    int i, j, k;

    for (i=0; i<dominance.preorder.count; i++)
    {
        struct basicblock* bb = dominance.preorder.item[i];
        register_assignment_t* old = bb->regsin;
        
        tracef('R', "R: considering block %s\n", bb->name);

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
         * allocate a new register for that phi value.
         */

        for (j=0; j<bb->phis.count; j++)
        {
            struct vreg* vreg = bb->phis.item[j].left;
            struct phi* phi = bb->phis.item[j].right;
            if (!pmap_findright(old, vreg))
            {
                struct phicongruence* c = vreg->congruence;
                struct hreg* hreg = allocate_phi_hreg(old, vreg, c->type);

                tracef('R', "R: import fallback hreg %s for phi input %%%d from %s\n",
                    hreg->name, vreg->id, phi->prev->name);
                pmap_add(old, hreg, vreg);
            }
        }
            
        for (j=0; j<bb->hops.count; j++)
        {
            struct hop* hop = bb->hops.item[j];
            register_assignment_t* in = &hop->regsin;
            register_assignment_t* out = &hop->regsout;;

            hop_print('R', hop);

            select_registers(hop, old, in, out);

            tracef('R', "R: %d from $%d: [", hop->id, hop->ir->id);
            for (k=0; k<hop->regsin.count; k++)
            {
                struct hreg* hreg = hop->regsin.item[k].left;
                struct vreg* vreg = hop->regsin.item[k].right;
                if (k != 0)
                    tracef('R', " ");
                tracef('R', "%%%d=>%s", vreg->id, hreg->name);
            }
            tracef('R', "] [");
            for (k=0; k<hop->regsout.count; k++)
            {
                struct hreg* hreg = hop->regsout.item[k].left;
                struct vreg* vreg = hop->regsout.item[k].right;
                if (k != 0)
                    tracef('R', " ");
                tracef('R', "%%%d=>%s", vreg->id, hreg->name);
            }
            tracef('R', "]\n");

            if (j > 0)
                j += insert_moves(bb, j, old, in);

            old = out;
        }
    }
}

static struct hop* create_swap(struct basicblock* bb, struct hreg* src, struct hreg* dest)
{
    struct hop* hop = new_hop(bb, NULL);

    hop_add_string_insel(hop, "! swap ");
    hop_add_hreg_insel(hop, src);
    hop_add_string_insel(hop, " <-> ");
    hop_add_hreg_insel(hop, dest);
    hop_add_eoi_insel(hop);

    return hop;
}

/* returns the number of instructions inserted */
static int insert_moves(struct basicblock* bb, int index,
    register_assignment_t* srcregs, register_assignment_t* destregs)
{
    int i;
    int inserted = 0;
    static PMAPOF(struct hreg, struct hreg) copies;

    copies.count = 0;
    for (i=0; i<destregs->count; i++)
    {
        struct hreg* dest = destregs->item[i].left;
        struct vreg* vreg = destregs->item[i].right;
        struct hreg* src = pmap_findright(srcregs, vreg);
        assert(src != NULL);

        if (src != dest)
            pmap_add(&copies, src, dest);
    }

    while (copies.count > 0)
    {
        struct hreg* src;
        struct hreg* dest;
        struct hreg* temp;
        struct hop* hop;

        /* Try and find a destination which isn't a source. */

        src = NULL;
        for (i=0; i<copies.count; i++)
        {
            dest = copies.item[i].right;
            if (!pmap_findleft(&copies, dest))
            {
                src = copies.item[i].left;
                break;
            }
        }

        if (src)
        {
            /* Copy. */

            hop = platform_move(bb, src, dest);
            pmap_remove(&copies, src, dest);
        }
        else
        {
            /* Swap. */
            
            assert(false);
            src = copies.item[0].left;
            dest = pmap_findleft(&copies, src);
            hop = create_swap(bb, src, dest);
            pmap_remove(&copies, src, dest);
            pmap_remove(&copies, dest, src);
        }

        array_insert(&bb->hops, hop, index + inserted);
        inserted++;
    }

    return inserted;
}

static void insert_phi_copies(void)
{
    int i, j, k;

    /* If we're importing an hreg from a parent block via a phi, insert a move
     * at the end of the parent block to put the result into the right
     * register. */

    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];

        /* Group together copies from each predecessor, so we can generate the
         * appropriate parallel move. */

        for (j=0; j<bb->prevs.count; j++)
        {
            struct basicblock* prevbb = bb->prevs.item[j];
            static register_assignment_t destregs;

            tracef('R', "R: inserting phis for %s -> %s\n",
                prevbb->name, bb->name);
            destregs.count = 0;
            for (k=0; k<bb->phis.count; k++)
            {
                struct vreg* vreg = bb->phis.item[k].left;
                struct phi* phi = bb->phis.item[k].right;
                struct hreg* dest = pmap_findright(bb->regsin, vreg);

                if ((phi->prev == prevbb) && dest)
                {
                    /* We inserted critical edges to guarantee this. */
                    assert(prevbb->nexts.count == 1);

                    tracef('R', "R: map %%%d -> %%%d (%s)\n",
                        phi->ir->result->id, 
                        vreg->id, dest->name);

                    pmap_put(&destregs, dest, phi->ir->result);
                }
            }

            /* Add any non-phi inputs. */

            for (k=0; k<bb->regsin->count; k++)
            {
                struct hreg* hreg = bb->regsin->item[k].left;
                struct vreg* vreg = bb->regsin->item[k].right;
                if (!pmap_findleft(&bb->phis, vreg))
                    pmap_add(&destregs, hreg, vreg);
            }

            /* The last instruction of a block should be the jump that sends us
             * to the next block. Insert the moves before then. */

            insert_moves(prevbb, prevbb->hops.count-1, prevbb->regsout, &destregs);
        }
    }
}

static int pack_stackframe(int stacksize, int size, uint32_t attr)
{
    int i;

    for (i=0; i<hregs.count; i++)
    {
        struct hreg* hreg = hregs.item[i];
        if (hreg->is_stacked && (hreg->type & attr))
        {
            hreg->offset = stacksize;
            stacksize += size;
        }
    }

    return stacksize;
}

static void layout_stack_frame(void)
{
    int stacksize = 0;
    stacksize = pack_stackframe(stacksize, 8, burm_bytes8_ATTR);
    stacksize = pack_stackframe(stacksize, 4, burm_bytes4_ATTR);
    stacksize = pack_stackframe(stacksize, 2, burm_bytes2_ATTR);
    stacksize = pack_stackframe(stacksize, 1, burm_bytes1_ATTR);
    current_proc->spills_size = stacksize;
}

void pass_register_allocator(struct procedure* proc)
{
    current_proc = proc;

    populate_hregs();
    wire_up_blocks_ins_outs();

    assign_hregs_to_vregs();
    insert_phi_copies();
    layout_stack_frame();
}

/* vim: set sw=4 ts=4 expandtab : */

