#include "mcg.h"

static void print_blocks(char k, struct procedure* proc)
{
	int i;

	tracef(k, "%c: procedure %s\n", k, proc->name);
	for (int i=0; i<proc->blocks_count; i++)
	{
		struct basicblock* bb = proc->blocks[i];
		int j;

		tracef(k, "%c: block %s\n", k, bb->name);

		for (int j=0; j<bb->inblocks_count; j++)
		{
			struct basicblock* obb = bb->inblocks[j];
			tracef(k, "%c:   %s ->\n", k, obb->name);
		}

		for (int j=0; j<bb->irs_count; j++)
			ir_print(k, bb->irs[j]);

		for (int j=0; j<bb->outblocks_count; j++)
		{
			struct basicblock* obb = bb->outblocks[j];
			tracef(k, "%c:   -> %s\n", k, obb->name);
		}

	}
}

void compile(struct procedure* proc)
{
	int i;

	print_blocks('1', proc);
}

/* vim: set sw=4 ts=4 expandtab : */

