#include "mcg.h"

void pass_splice_adjacent_blocks(struct procedure* proc)
{
    int i, j;

again:
    for (i=0; i<proc->blocks_count; i++)
    {
        struct basicblock* bb = proc->blocks[i];
        if (bb->outblocks_count == 1)
        {
            struct basicblock* outbb = bb->outblocks[0];
            if (outbb->inblocks_count == 1)
            {
                struct ir* lastir = bb->irs[bb->irs_count-1];

                if ((lastir->opcode == IR_JUMP)
                    && (lastir->left->opcode == IR_BLOCK)
                    && (lastir->left->u.bvalue == outbb))
                {
                    /* Remove jump instruction. */

                    bb->irs_count--; 

                    REMOVE(bb->outblocks, outbb);
                    REMOVE(outbb->inblocks, bb);

                    for (j=0; j<outbb->irs_count; j++)
                        APPEND(bb->irs, outbb->irs[j]);
                    for (j=0; j<outbb->outblocks_count; j++)
                    {
                        APPENDU(bb->outblocks, outbb->outblocks[j]);
                        APPENDU(outbb->outblocks[j]->inblocks, bb);
                        REMOVE(outbb->outblocks[j]->inblocks, outbb);
                    }

                    REMOVE(proc->blocks, outbb);
                    goto again;
                }
            }
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */

