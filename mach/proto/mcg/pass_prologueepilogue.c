#include "mcg.h"

void pass_add_prologue_epilogue(struct procedure* proc)
{
	struct hop* prologue = platform_prologue(proc);
	array_insert(&proc->entry->hops, prologue, 0);

	if (proc->exit)
	{
		struct hop* epilogue = platform_epilogue(proc);

		proc->exit->hops.count = 0;
		array_append(&proc->exit->hops, epilogue);
	}
}

/* vim: set sw=4 ts=4 expandtab : */


