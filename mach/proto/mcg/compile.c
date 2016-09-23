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

void compile(struct procedure* proc)
{
	int i;

	print_blocks('1', proc);

    pass_remove_dead_blocks(proc);
    pass_convert_stack_ops(proc);
    pass_splice_adjacent_blocks(proc);

    print_blocks('2', proc);
}

/* vim: set sw=4 ts=4 expandtab : */

