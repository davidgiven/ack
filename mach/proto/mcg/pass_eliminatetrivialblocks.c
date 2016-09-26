#include "mcg.h"

static bool rewrite_jumps_cb(struct ir* ir, void* user)
{
    if (ir->opcode == IR_BLOCK)
    {
        struct basicblock* bb = ir->u.bvalue;
		if (!bb->is_fake
			&& (bb->irs.count > 0)
            && (bb->irs.item[0]->opcode == IR_JUMP)
            && (bb->irs.item[0]->left->opcode == IR_BLOCK))
        {
            ir->u.bvalue = bb->irs.item[0]->left->u.bvalue;
        }
    }

    return false;
}

static void rewrite_jumps(struct basicblock* bb)
{
    int i;

    for (i=0; i<bb->irs.count; i++)
    {
        struct ir* ir = bb->irs.item[i];
        ir_walk(ir, rewrite_jumps_cb, NULL);
    }
}

void pass_eliminate_trivial_blocks(struct procedure* proc)
{
    int i;

    for (i=0; i<proc->blocks.count; i++)
    {
        struct basicblock* bb = proc->blocks.item[i];
        rewrite_jumps(bb);
    }
}

/* vim: set sw=4 ts=4 expandtab : */
