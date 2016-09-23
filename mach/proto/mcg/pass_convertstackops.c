#include "mcg.h"

STATICMAP(graph);
STATICARRAY(struct ir, pops);
STATICARRAY(struct ir, pushes);

static struct ir* get_last_push(struct basicblock* bb)
{
    int i;

    for (i=bb->irs_count-1; i>=0; i--)
    {
        struct ir* ir = bb->irs[i];

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

    for (i=0; i<bb->irs_count; i++)
    {
        struct ir* irr;
        struct ir* ir = bb->irs[i];

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
        MAP_SET(graph, caller, ir->u.bvalue);
    return false;
}

static void make_bb_graph(struct procedure* proc)
{
    int i, j;

    graph_count = 0;
    for (i=0; i<proc->blocks_count; i++)
    {
        struct basicblock* bb = proc->blocks[i];
        for (j=0; j<bb->irs_count; j++)
            ir_walk(bb->irs[j], collect_outputs_cb, bb);
    }
}

static void convert_block(struct procedure* proc, struct basicblock* bb)
{
    int i, j;
    struct ir* ir;

    pushes_count = pops_count = 0;
    for (;;)
    {
        struct ir* lastpush = get_last_push(bb);
        if (!lastpush)
            return;

        /* Abort unless *every* successor block of this one starts with a pop
         * of the same size... */

        for (i=0; i<graph_count; i++)
        {
            if (graph[i].left == bb)
            {
                struct basicblock* outbb = graph[i].right;

                ir = get_first_pop(outbb);
                if (!ir || (ir->size != lastpush->size))
                    return;
                APPENDU(pops, ir);

                /* Also abort unless *every* predecessor block of the one we've
                 * just found *also* ends in a push of the same size. */

                for (j=0; j<graph_count; j++)
                {
                    if (graph[j].right == outbb)
                    {
                        struct basicblock* inbb = graph[j].left;

                        ir = get_last_push(inbb);
                        if (!ir || (ir->size != lastpush->size))
                            return;
                        APPENDU(pushes, ir);
                    }
                }
            }
        }

        /* Okay, now we can wire them all up. */

        for (i=0; i<pushes_count; i++)
        {
            struct ir* ir = pushes[i];
            assert(ir->is_sequence);
            *ir = *ir->left;
            ir->is_sequence = true;
        }

        for (i=0; i<pops_count; i++)
        {
            struct ir* ir = pops[i];
            struct ir* pushir = pushes[0];
            struct ir* phi = new_ir1(IR_PHI, ir->size, pushir);

            for (j=1; j<pushes_count; j++)
                phi = new_ir2(IR_PHI, ir->size, phi, pushes[j]);

            phi->is_sequence = ir->is_sequence;
            *ir = *phi;
        }
    }
}

void pass_convert_stack_ops(struct procedure* proc)
{
    int i;

    make_bb_graph(proc);

    for (i=0; i<proc->blocks_count; i++)
        convert_block(proc, proc->blocks[i]);
}

/* vim: set sw=4 ts=4 expandtab : */
