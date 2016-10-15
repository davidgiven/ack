#include "mcg.h"

struct hop* platform_prologue(struct procedure* proc)
{
	int framesize = proc->nlocals + 8;
	int retbase = proc->nlocals;

	struct hop* hop = new_hop(proc->entry, NULL);

	hop_add_insel(hop, "addi sp, fp, %d", -framesize);
	hop_add_insel(hop, "mfspr 0, lr");
	hop_add_insel(hop, "stw fp, %d(sp)", retbase);
	hop_add_insel(hop, "stw 0, %d(sp)", retbase+4);
	hop_add_insel(hop, "addi fp, sp, retbase");

	return hop;
}

struct hop* platform_epilogue(struct procedure* proc)
{
	struct hop* hop = new_hop(proc->exit, NULL);

	hop_add_insel(hop, "b .ret");

	return hop;
}

struct hop* platform_move(struct basicblock* bb, struct hreg* src, struct hreg* dest)
{
    struct hop* hop = new_hop(bb, NULL);

	if ((src->type & burm_int_ATTR) && (dest->type & burm_int_ATTR))
		hop_add_insel(hop, "mr %H, %H", dest, src);
	else
		fatal("cannot generate move from %s to %s", src->name, dest->name);

    return hop;
}

/* vim: set sw=4 ts=4 expandtab : */

