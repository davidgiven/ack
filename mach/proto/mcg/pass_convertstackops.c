#include "mcg.h"

static PMAPOF(struct basicblock, struct basicblock) graph;
static ARRAYOF(struct ir) pops;
static ARRAYOF(struct ir) pushes;

static struct ir* get_last_push(struct basicblock* bb)
{
    int i;

    for (i=bb->irs.count-1; i>=0; i--)
    {
        struct ir* ir = bb->irs.item[i];

        if (ir->opcode == IR_PUSH)
            return ir;
        if (ir_find(ir, IR_POP))
            return NULL;
    }

    return NULL;
}

static struct ir* get_first_pop(struct basicblock* bb)
{
    int i;

    for (i=0; i<bb->irs.count; i++)
    {
        struct ir* irr;
        struct ir* ir = bb->irs.item[i];

        if (ir->opcode == IR_PUSH)
            return NULL;

        irr = ir_find(ir, IR_POP);
        if (irr)
            return irr;
    }

    return NULL;
}

static bool collect_outputs_cb(struct ir* ir, void* user)
{
    struct basicblock* caller = user;

    if (ir->opcode == IR_BLOCK)
        pmap_add(&graph, caller, ir->u.bvalue);
    return false;
}

static void make_bb_graph(struct procedure* proc)
{
    int i, j;

    graph.count = 0;
    for (i=0; i<proc->blocks.count; i++)
    {
        struct basicblock* bb = proc->blocks.item[i];
        for (j=0; j<bb->irs.count; j++)
            ir_walk(bb->irs.item[j], collect_outputs_cb, bb);
    }
}

static void convert_block(struct procedure* proc, struct basicblock* bb)
{
    int i, j;
    struct ir* ir;

    pushes.count = pops.count = 0;
    for (;;)
    {
        struct ir* lastpush = get_last_push(bb);
        if (!lastpush)
            return;

        /* Abort unless *every* successor block of this one starts with a pop
         * of the same size... */

        for (i=0; i<graph.count; i++)
        {
            if (graph.item[i].left == bb)
            {
                struct basicblock* outbb = graph.item[i].right;

                ir = get_first_pop(outbb);
                if (!ir || (ir->size != lastpush->size))
                    return;
                array_appendu(&pops, ir);

                /* Also abort unless *every* predecessor block of the one we've
                 * just found *also* ends in a push of the same size. */

                for (j=0; j<graph.count; j++)
                {
                    if (graph.item[j].right == outbb)
                    {
                        struct basicblock* inbb = graph.item[j].left;

                        ir = get_last_push(inbb);
                        if (!ir || (ir->size != lastpush->size))
                            return;
                        array_appendu(&pushes, ir);
                    }
                }
            }
        }

        /* Okay, now we can wire them all up. */

        for (i=0; i<pushes.count; i++)
        {
            struct ir* ir = pushes.item[i];
            *ir = *ir->left;
        }

        for (i=0; i<pops.count; i++)
        {
            struct ir* ir = pops.item[i];
            struct ir* phi = new_ir0(IR_PHI, ir->size);

            for (j=0; j<pushes.count; j++)
                array_appendu(&phi->u.phivalue, pushes.item[j]);
            phi->root = phi;

            *ir = *phi;
            array_insert(&bb->irs, ir, 0);
        }
    }
}

void pass_convert_stack_ops(struct procedure* proc)
{
    int i;

    make_bb_graph(proc);

    for (i=0; i<proc->blocks.count; i++)
        convert_block(proc, proc->blocks.item[i]);
}

/* vim: set sw=4 ts=4 expandtab : */
