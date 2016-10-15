#include "mcg.h"

/* mcg stack frames are laid out as:
 *
 * |    ...params...
 * |  --------------- <- ab
 * |     saved regs
 * |  ---------------
 * |      spills 
 * |  --------------- <- st, fp (a.k.a. lb)
 * |      locals
 * |  --------------- <- sp
 * V  ...user area...
 *
 * st indexes up; lb indexes down.
 */

void platform_calculate_offsets(void)
{
	current_proc->fp_to_st = 0;
	current_proc->fp_to_ab = current_proc->spills_size + current_proc->saved_size + 8;
	current_proc->fp_to_lb = 0;
}

struct hop* platform_prologue(void)
{
	struct hop* hop = new_hop(current_proc->entry, NULL);

	hop_add_insel(hop, "! saved_size = %d+8 bytes", current_proc->saved_size);
	hop_add_insel(hop, "! spills_size = %d bytes", current_proc->spills_size);
	hop_add_insel(hop, "! locals_size = %d bytes", current_proc->locals_size);
	hop_add_insel(hop, "addi sp, sp, %d", -(current_proc->fp_to_ab + current_proc->locals_size));
	hop_add_insel(hop, "mfspr 0, lr");
	hop_add_insel(hop, "stw fp, %d(sp)", current_proc->fp_to_st + current_proc->locals_size);
	hop_add_insel(hop, "stw 0, %d(sp)", current_proc->fp_to_st + current_proc->locals_size + 4);
	hop_add_insel(hop, "addi fp, sp, %d", current_proc->locals_size);

	return hop;
}

struct hop* platform_epilogue(void)
{
	struct hop* hop = new_hop(current_proc->exit, NULL);

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

