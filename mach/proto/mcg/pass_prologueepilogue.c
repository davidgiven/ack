#include "mcg.h"

void pass_add_prologue_epilogue(void)
{
    int i, j, k;

    current_proc->usedregs.count = 0;
    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];

        for (j=0; j<bb->hops.count; j++)
        {
            struct hop* hop = bb->hops.item[j];

            for (k=0; k<hop->regsin.count; k++)
                array_appendu(&current_proc->usedregs, hop->regsin.item[k].left);

            for (k=0; k<hop->regsout.count; k++)
                array_appendu(&current_proc->usedregs, hop->regsout.item[k].left);
        }
    }

	platform_calculate_offsets();

	array_insert(&current_proc->entry->hops, platform_prologue(), 0);

	if (current_proc->exit)
	{
		current_proc->exit->hops.count = 0;
		array_append(&current_proc->exit->hops, platform_epilogue());
	}
}

/* vim: set sw=4 ts=4 expandtab : */

