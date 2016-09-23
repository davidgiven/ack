#include "mcg.h"

STATICARRAY(struct basicblock, used);

static void walk_blocks(struct basicblock* bb);

static bool walk_blocks_cb(struct ir* ir, void* user)
{
    if (ir->opcode == IR_BLOCK)
        walk_blocks(ir->u.bvalue);
    return false;
}

static void walk_blocks(struct basicblock* bb)
{
    int i;

    if (!CONTAINS(used, bb))
    {
        APPENDU(used, bb);

        for (i=0; i<bb->irs_count; i++)
            ir_walk(bb->irs[i], walk_blocks_cb, NULL);
    }
}

void pass_remove_dead_blocks(struct procedure* proc)
{
    int i, j;
    
    used_count = 0;
    walk_blocks(proc->blocks[0]);

    proc->blocks_count = 0;
    for (i=0; i<used_count; i++)
        APPEND(proc->blocks, used[i]);
}

/* vim: set sw=4 ts=4 expandtab : */

