#include "mcg.h"

/* mcg stack frames are laid out as:
 *
 * |    ...params...
 * |  --------------- <- ap
 * |     saved regs
 * |  --------------- <- st
 * |      spills 
 * |  --------------- <- fp (a.k.a. lb)
 * |      locals
 * |  --------------- <- sp
 * V  ...user area...
 *
 */

void platform_calculate_offsets(struct procedure* proc)
{
	proc->fp_to_st = proc->spills_size;
	proc->fp_to_ap = proc->fp_to_st + proc->saved_size + 8;
	proc->fp_to_lb = 0;
}

struct hop* platform_prologue(struct procedure* proc)
{
	struct hop* hop = new_hop(proc->entry, NULL);

	hop_add_insel(hop, "addi sp, sp, %d", proc->fp_to_ap + proc->locals_size);
	hop_add_insel(hop, "mfspr 0, lr");
	hop_add_insel(hop, "stw fp, %d(sp)", proc->fp_to_st + proc->locals_size);
	hop_add_insel(hop, "stw 0, %d(sp)", proc->fp_to_st + proc->locals_size + 4);
	hop_add_insel(hop, "addi fp, sp, %d", proc->locals_size);

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
	{
		if (src->is_stacked)
			hop_add_insel(hop, "lwz %H, %S(fp) ! %H", dest, src, src);
		else if (dest->is_stacked)
			hop_add_insel(hop, "stw %H, %S(fp) ! %H", src, dest, dest);
		else
			hop_add_insel(hop, "mr %H, %H", dest, src);
	}
	else
		fatal("cannot generate move from %s to %s", src->name, dest->name);

    return hop;
}

/* vim: set sw=4 ts=4 expandtab : */

