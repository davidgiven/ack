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

        if (bb->prevs.count > 0)
        {
            tracef(k, "%c: FROM:", k);
            for (j=0; j<bb->prevs.count; j++)
                tracef(k, " %s", bb->prevs.item[j]->name);
            tracef(k, "\n");
        }

        if (bb->nexts.count > 0)
        {
            tracef(k, "%c:   TO:", k);
            for (j=0; j<bb->nexts.count; j++)
                tracef(k, " %s", bb->nexts.item[j]->name);
            tracef(k, "\n");
        }

		for (j=0; j<bb->irs.count; j++)
			ir_print(k, bb->irs.item[j]);
	}
}

void procedure_compile(struct procedure* proc)
{
	int i;

	print_blocks('1', proc);

    pass_group_irs(proc);
    /* Passes from here on must preserve IR grouping */

    pass_eliminate_trivial_blocks(proc);
    pass_remove_dead_blocks(proc);

    update_graph_data(proc);

    /* Passes from here on can't alter the BB graph without also updating prevs
     * and nexts (and then calling update_graph_data()). */

    print_blocks('2', proc);
    pass_convert_stack_ops(proc);
    print_blocks('3', proc);
    pass_convert_locals_to_ssa(proc);
    print_blocks('4', proc);
    pass_promote_float_ops(proc);
    print_blocks('5', proc);
    pass_split_critical_edges(proc);
    print_blocks('6', proc);

    update_graph_data(proc);

    pass_instruction_selector(proc);
    pass_register_allocator(proc);
}

/* vim: set sw=4 ts=4 expandtab : */

