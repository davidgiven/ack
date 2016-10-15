#include "mcg.h"

void pass_add_prologue_epilogue(void)
{
	platform_calculate_offsets();

	array_insert(&current_proc->entry->hops, platform_prologue(), 0);

	if (current_proc->exit)
	{
		current_proc->exit->hops.count = 0;
		array_append(&current_proc->exit->hops, platform_epilogue());
	}
}

/* vim: set sw=4 ts=4 expandtab : */

