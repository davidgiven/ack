#include "mcg.h"

static void print_blocks(char k, struct procedure* proc)
{
	int i;

	tracef(k, "%c: procedure %s\n", k, proc->name);
	for (int i=0; i<proc->blocks.count; i++)
	{
		struct basicblock* bb = proc->blocks.item[i];
		int j;

        tracef(k, "%c:\n", k);
		tracef(k, "%c: %sBLOCK: %s\n", k,
            bb->is_fake ? "FAKE " : "",
            bb->name);

		for (int j=0; j<bb->irs.count; j++)
			ir_print(k, bb->irs.item[j]);
	}
}

void procedure_compile(struct procedure* proc)
{
	int i;

	print_blocks('1', proc);

    pass_eliminate_trivial_blocks(proc);
    pass_remove_dead_blocks(proc);
    pass_convert_stack_ops(proc);

    print_blocks('2', proc);

    pass_instruction_selector(proc);
}

/* vim: set sw=4 ts=4 expandtab : */

