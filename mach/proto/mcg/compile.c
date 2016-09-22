#include "mcg.h"

static void print_blocks(char k, struct procedure* proc)
{
	int i;

	tracef(k, "%c: procedure %s\n", k, proc->name);
	for (int i=0; i<proc->blocks_count; i++)
	{
		struct basicblock* bb = proc->blocks[i];
		int j;

        tracef(k, "%c:\n", k);
		tracef(k, "%c: BLOCK: %s\n", k, bb->name);

        tracef(k, "%c:  from:", k);
		for (int j=0; j<bb->inblocks_count; j++)
		{
			struct basicblock* obb = bb->inblocks[j];
			tracef(k, " %s",  obb->name);
		}
        tracef(k, "\n");
        tracef(k, "%c:    to:", k);
		for (int j=0; j<bb->outblocks_count; j++)
		{
			struct basicblock* obb = bb->outblocks[j];
			tracef(k, " %s", obb->name);
		}
        tracef(k, "\n");

		for (int j=0; j<bb->irs_count; j++)
			ir_print(k, bb->irs[j]);

	}
}

static void remove_dead_blocks(struct procedure* proc)
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

static void splice_adjacent_blocks(struct procedure* proc)
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

void compile(struct procedure* proc)
{
	int i;

	print_blocks('1', proc);

    remove_dead_blocks(proc);

    for (i=0; i<proc->blocks_count; i++)
        sse_convert_block_parameters(proc->blocks[i]);

    splice_adjacent_blocks(proc);

    print_blocks('2', proc);
}

/* vim: set sw=4 ts=4 expandtab : */

