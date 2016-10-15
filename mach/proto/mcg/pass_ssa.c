#include "mcg.h"

static PMAPOF(struct basicblock, struct basicblock) dominancefrontiers;

static struct local* current_local;
static ARRAYOF(struct basicblock) defining;
static ARRAYOF(struct basicblock) needsphis;
static ARRAYOF(struct ir) definitions;
static ARRAYOF(struct basicblock) rewritten;

static void calculate_dominance_frontier_graph(void)
{
    /* This is the algorithm described here:
     *
     * Cooper, Keith D., Timothy J. Harvey, and Ken Kennedy.
     * "A simple, fast dominance algorithm."
     * Software Practice & Experience 4.1-10 (2001): 1-8.
     *
     * https://www.cs.rice.edu/~keith/EMBED/dom.pdf
     */

    int i, j;

    dominancefrontiers.count = 0;

    for (i=0; i<cfg.postorder.count; i++)
    {
        struct basicblock* b = cfg.postorder.item[i];
        struct basicblock* dominator = pmap_findleft(&dominance.graph, b);
        if (b->prevs.count >= 2)
        {
            for (j=0; j<b->prevs.count; j++)
            {
                struct basicblock* runner = b->prevs.item[j];
                while (runner != dominator)
                {
                    tracef('S', "S: %s is in %s's dominance frontier\n",
                        b->name, runner->name);
                    pmap_add(&dominancefrontiers, runner, b);
                    runner = pmap_findleft(&dominance.graph, runner);
                }
            }
        }
    }

}

static bool is_local(struct ir* ir)
{
    return ((ir->opcode == IR_LOAD) &&
            (ir->left->opcode == IR_LOCAL) &&
            (ir->left->u.ivalue == current_local->offset));
}

static bool rewrite_loads_cb(struct ir* ir, void* user)
{
    struct ir* definition = user;

    /* Rewrite in place where possible. */

    if (ir->left && is_local(ir->left))
        ir->left = definition;
    if (ir->right && is_local(ir->right))
        ir->right = definition;

    /* Otherwise, go via a IR_REG (which should, with luck, turn into no code). */
    if (is_local(ir))
    {
        ir->opcode = IR_NOP;
        ir->size = 0;
        ir->left = definition;
        ir->right = NULL;
    }

    return false;
}

/* Walks the tree, rewriting IRs to push new definitions downwards. */

static void recursively_rewrite_tree(struct basicblock* bb)
{
    int i;
    int defcount = definitions.count;

    if (array_contains(&rewritten, bb))
        return;
    array_appendu(&rewritten, bb);

    for (i=0; i<bb->irs.count; i++)
    {
        struct ir* ir = bb->irs.item[i];
        
        if (definitions.count > 0)
        {
            ir_walk(ir, rewrite_loads_cb, definitions.item[definitions.count-1]);
        }

        if (((ir->opcode == IR_STORE) &&
             (ir->left->opcode == IR_LOCAL) &&
             (ir->left->u.ivalue == current_local->offset)
            ) ||
            ((i == 0) &&
             (ir->opcode == IR_PHI) &&
             array_contains(&needsphis, bb)))
        {
            /* This is a definition. */

            if (ir->opcode == IR_STORE)
            {
                ir->opcode = IR_NOP;
                ir->size = 0;
                ir->left = ir->right;
                ir->right = NULL;
            }
            array_push(&definitions, ir);
        }
    }

    for (i=0; i<bb->nexts.count; i++)
    {
        struct basicblock* nextbb = bb->nexts.item[i];
        struct ir* ir = nextbb->irs.item[0];

        if ((definitions.count > 0) &&
            (ir->opcode == IR_PHI) &&
            array_contains(&needsphis, nextbb))
        {
            pmap_add(&ir->u.phivalue,
                bb, definitions.item[definitions.count-1]);
        }

        recursively_rewrite_tree(nextbb);
    }

    definitions.count = defcount;
}

static void ssa_convert(void)
{
    int i, j;

    /* If this is a parameter, synthesise a load/store at the beginning of the
     * program to force it into a register. (Unless it's written to it'll
     * always be read from the frame.) */

    if (current_local->offset >= 0)
    {
        struct ir* ir = new_ir2(
            IR_STORE, current_local->size,
            new_localir(current_local->offset),
            new_ir1(
                IR_LOAD, current_local->size,
                new_localir(current_local->offset)
            )
        );

        ir->root = ir;
        ir->left->root = ir;
        ir->right->root = ir;
        ir->right->left->root = ir;
        array_insert(&cfg.entry->irs, ir, 0);
    }

    defining.count = 0;
    needsphis.count = 0;

    /* Find everwhere where the variable is *defined*. */

    for (i=0; i<cfg.postorder.count; i++)
    {
        struct basicblock* bb = cfg.postorder.item[i];
        for (j=0; j<bb->irs.count; j++)
        {
            struct ir* ir = bb->irs.item[j];
            if ((ir->opcode == IR_STORE) &&
                (ir->left->opcode == IR_LOCAL) &&
                (ir->left->u.ivalue == current_local->offset))
            {
                array_appendu(&defining, bb);
            }
        }
    }

    /* Every block which is in one of the defining block's dominance frontiers
     * requires a phi. Remember that adding a phi also adds a definition. */

    for (i=0; i<defining.count; i++)
    {
        struct basicblock* bb = defining.item[i];
        struct basicblock* dominates = pmap_findleft(&dominancefrontiers, bb);
        if (dominates)
        {
            array_appendu(&needsphis, dominates);
            array_appendu(&defining, dominates);
            tracef('S', "S: local %d needs phi in block %s\n", current_local->offset, dominates->name);
        }
    }

    /* Add empty phi nodes. */

    for (i=0; i<needsphis.count; i++)
    {
        struct basicblock* bb = needsphis.item[i];
        struct ir* ir = new_ir0(IR_PHI, current_local->size);
        ir->root = ir;
        array_insert(&bb->irs, ir, 0);
    }

    /* Now do the rewriting by walking the tree, pushing definitions down the tree. */

    definitions.count = 0;
    rewritten.count = 0;
    recursively_rewrite_tree(cfg.entry);
}

void pass_convert_locals_to_ssa(void)
{
    int i;

    calculate_dominance_frontier_graph();

    for (i=0; i<current_proc->locals.count; i++)
    {
        current_local = current_proc->locals.item[i].right;
        if (current_local->is_register)
            ssa_convert();
    }
}

/* vim: set sw=4 ts=4 expandtab : */


