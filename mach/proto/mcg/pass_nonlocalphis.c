#include "mcg.h"

/* Phis can import IRs defined in any IR which strictly dominates the current
 * one. This makes life awkward for future passes, as it's not obvious what's
 * defined where, so we convert these non-local references into phis,
 * so ensuring that all IR references are to either the current bb, or via
 * a phi to a parent bb. */

static struct set confirmed;
static struct set pending;
static struct graph added;

static struct basicblock* current_src;
static struct basicblock* current_dest;
static struct ir* current_ir;

static bool is_defined_in_prev_block(struct basicblock* bb, struct ir* ir)
{
    int i;

    for (i=0; i<bb->prevs.count; i++)
        if (bb->prevs.item[i] == ir->bb)
            return true;

    return false;
}

static void recursively_add_children_to_pending(struct basicblock* bb)
{
    int i;

    for (i=0; i<bb->prevs.count; i++)
    {
        struct basicblock* candidate = bb->prevs.item[i];

        if ((candidate != current_src)
                && (candidate != current_dest)
                && !set_add(&pending, candidate))
            recursively_add_children_to_pending(candidate);
    }
}

static void recursively_move_children_to_confirmed(struct basicblock* bb)
{
    int i;

    for (i=0; i<bb->nexts.count; i++)
    {
        struct basicblock* candidate = bb->nexts.item[i];

        if (set_contains(&pending, candidate))
        {
            tracef('I', "I: encompassing %s\n", candidate->name);
            set_remove(&pending, candidate);
            set_add(&confirmed, candidate);
            recursively_move_children_to_confirmed(candidate);
        }
    }
}

static struct ir* new_phi(struct basicblock* bb, int size, struct ir* src)
{
    struct ir* phi = new_ir0(IR_PHI, size);
    phi->root = phi;
    phi->bb = bb;

    if (src)
        pmap_add(&phi->u.phivalue, src->bb, src);
    array_insert(&bb->irs, phi, 0);

    return phi;
}

static struct ir* insert_phi_to_prev(struct basicblock* bb, int size, struct ir* src)
{
    struct ir* phi = new_phi(bb, size, src);
    tracef('I', "I: adding phi $%d for $%d in %s\n",
        phi->id, src->id, bb->name);
    ir_rewrite_single_block(bb, src, phi);
    return phi;
}

static bool replace_irs_cb(struct ir* ir, void* user)
{
    int i;
    bool* found = user;

    if (*found)
        return true;
    if (ir->opcode != IR_PHI)
        return true;
         
    for (i=0; i<ir->u.phivalue.count; i++)
    {
        if (graph_contains_edge(&added, current_ir, ir->u.phivalue.item[i].right))
        {
            *found = true;
            return true;
        }
    }

    return false;
}

static bool already_importing(struct basicblock* bb)
{
    int i;

    for (i=0; i<bb->irs.count; i++)
    {
        bool found = false;
        ir_walk(bb->irs.item[i], replace_irs_cb, &found);
        if (found)
            return true;
    }

    return false;
}

static void import_ir(struct ir* phi)
{
    int i;

    set_empty(&confirmed);
    set_empty(&pending);

    recursively_add_children_to_pending(current_dest);
    set_add(&confirmed, current_dest);
    recursively_move_children_to_confirmed(current_src);

    /* Remove the original source from the phi. */

    ir_print('I', phi);
    pmap_remove_either(&phi->u.phivalue, current_ir);

    /* For every prev which is in our confirmed list, make sure
     * that prev exports an IR, and add that IR to the phi. */

    for (i=0; i<current_dest->prevs.count; i++)
    {
        struct basicblock* bb = current_dest->prevs.item[i];

        if (bb == current_src)
            pmap_add(&phi->u.phivalue, bb, current_ir);
        else if (set_contains(&confirmed, bb) && !already_importing(bb))
        {
            struct ir* newphi = insert_phi_to_prev(bb, current_ir->size, current_ir);
            pmap_add(&phi->u.phivalue, bb, newphi);
            graph_add_edge(&added, current_ir, newphi);
            set_remove(&confirmed, bb);
        }
    }
    ir_print('I', phi);
}

void pass_convert_nonlocal_phis(void)
{
    int i, j, k;

    graph_reset(&added);

    /* If a phi refers to an IR defined in a node which isn't a direct parent,
     * insert phis upstream for it. */

restart:
    for (i=0; i<dominance.preorder.count; i++)
    {
        current_dest = dominance.preorder.item[i];

        for (j=0; j<current_dest->irs.count; j++)
        {
            struct ir* phi = current_dest->irs.item[j];
            if (phi->opcode == IR_PHI)
            {
                for (k=0; k<phi->u.phivalue.count; k++)
                {
                    current_ir = phi->u.phivalue.item[k].right;
                    current_src = current_ir->bb;

                    if (!array_contains(&current_dest->prevs, current_src))
                    {
                        tracef('I', "I: import of non-local IR $%d into %s from %s\n",
                            current_ir->id, current_dest->name, current_src->name);
                        import_ir(phi);
                        goto restart;
                    }
                }
            }
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */
