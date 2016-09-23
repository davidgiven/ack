#include "mcg.h"

void pass_remove_dead_blocks(struct procedure* proc)
{
    int i, j;

again:
    /* Starts at 1 because we don't want to remove the root block! */
    for (i=1; i<proc->blocks_count; i++)
    {
        struct basicblock* bb = proc->blocks[i];
        
        if (bb->inblocks_count == 0)
        {
            /* Nobody uses this block; disconnect it from its output
             * blocks. */
            for (j=0; j<bb->outblocks_count; j++)
                REMOVE(bb->outblocks[j]->inblocks, bb);

            REMOVE(proc->blocks, bb);
            goto again;
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */

