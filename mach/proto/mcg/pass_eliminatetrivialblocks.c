#include "mcg.h"

static bool rewrite_jumps_cb(struct ir* ir, void* user)
{
    if (ir->opcode == IR_BLOCK)
    {
        struct basicblock* bb = ir->u.bvalue;
        if ((bb->irs_count > 0)
            && (bb->irs[0]->opcode == IR_JUMP)
            && (bb->irs[0]->left->opcode == IR_BLOCK))
        {
            ir->u.bvalue = bb->irs[0]->left->u.bvalue;
        }
    }

    return false;
}

static void rewrite_jumps(struct basicblock* bb)
{
    int i;

    for (i=0; i<bb->irs_count; i++)
    {
        struct ir* ir = bb->irs[i];
        ir_walk(ir, rewrite_jumps_cb, NULL);
    }
}

void pass_eliminate_trivial_blocks(struct procedure* proc)
{
    int i;

    for (i=0; i<proc->blocks_count; i++)
    {
        struct basicblock* bb = proc->blocks[i];
        rewrite_jumps(bb);
    }
}

/* vim: set sw=4 ts=4 expandtab : */
