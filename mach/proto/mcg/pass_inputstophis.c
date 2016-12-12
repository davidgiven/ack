#include "mcg.h"

/* Currently, IRs can refer to other IRs which are either in the current bb or
 * else defined in a parent bb (although, we hope, in a strictly dominating
 * bb). This makes life awkward in future passes, as it's not obvious where
 * each IR is defined. So, we convert these non-local references into phis,
 * so ensuring that all IR references are to either the current bb, or via
 * a phi to a parent bb. */

/* Tests if this node *contains* a non-local reference. */
static bool find_non_local_direct_reference_cb(struct ir* ir, void* user)
{
    if (ir->left && (ir->left->bb != ir->bb))
        return true;
    if (ir->right && (ir->right->bb != ir->bb))
        return true;
    return false;
}

/* Returns the non-local reference itself. */
static struct ir* find_non_local_direct_reference(struct basicblock* bb)
{
    int i;

    for (i=0; i<bb->irs.count; i++)
    {
        struct ir* ir = ir_walk(bb->irs.item[i],
            find_non_local_direct_reference_cb, bb);
        if (ir)
        {
            if (ir->left && (ir->left->bb != bb))
                return ir->left;
            if (ir->right && (ir->right->bb != bb))
                return ir->right;
        }
    }

    return NULL;
}

static bool is_defined_in_prev_block(struct basicblock* bb, struct ir* ir)
{
    int i;

    for (i=0; i<bb->prevs.count; i++)
        if (bb->prevs.item[i] == ir->bb)
            return true;

    return false;
}

static struct ir* insert_phis_and_rewrite(struct basicblock* bb, struct ir* src)
{
    int i;
    struct ir* dest;
    
    dest = new_ir0(IR_PHI, src->size);
    dest->root = dest;
    dest->bb = bb;

    tracef('I', "I: in %s, inserting phi $%d for $%d (from %s)\n",
        bb->name, dest->id, src->id, src->bb->name);

    pmap_add(&dest->u.phivalue, src->bb, src);
    array_insert(&bb->irs, dest, 0);

    ir_rewrite(bb, src, dest);

    if (is_defined_in_prev_block(bb, src))
        return dest;

    return dest;
}

void pass_convert_inputs_to_phis(void)
{
    int i;

    /* Insert new phi nodes for any use of a non-local IR in an ordinary
     * instruction. */

    for (i=0; i<dominance.preorder.count; i++)
    {
        struct basicblock* bb = dominance.preorder.item[i];

        for (;;)
        {
            struct ir* ir = find_non_local_direct_reference(bb);
            if (!ir)
                break;

            tracef('I', "I: considering $%d in %s, referring to %s\n",
                ir->id, bb->name, ir->bb->name);
            insert_phis_and_rewrite(bb, ir);
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */
