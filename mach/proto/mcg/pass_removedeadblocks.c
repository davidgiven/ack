#include "mcg.h"

static ARRAYOF(struct basicblock) used;

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

    if (!array_contains(&used, bb))
    {
        array_append(&used, bb);

        for (i=0; i<bb->irs.count; i++)
            ir_walk(bb->irs.item[i], walk_blocks_cb, NULL);
    }
}

void pass_remove_dead_blocks(void)
{
    int i, j;
    
    used.count = 0;
    walk_blocks(current_proc->blocks.item[0]);

    current_proc->blocks.count = 0;
    for (i=0; i<used.count; i++)
        array_append(&current_proc->blocks, used.item[i]);
}

/* vim: set sw=4 ts=4 expandtab : */

