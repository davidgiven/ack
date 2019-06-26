#include "mcg.h"

/* mcg stack frames are laid out as:
 *
 * |    ...params...
 * |  --------------- <- ab
 * |       old PC
 * |       old FP
 * |  --------------- <- fp (a.k.a. lb): iy
 * |      locals
 * |  ---------------
 * |      spills
 * |  --------------- <- sb
 * |     saved regs      (always empty on Z80)
 * |  --------------- <- sp, rb: sp
 * V  ...user area...
 *
 * st indexes up; lb indexes down.
 *
 * Note that [fp] == old_fp and ab == fp + 8.
 */

void platform_calculate_offsets(void)
{
	current_proc->fp_to_ab = 4;
	current_proc->fp_to_lb = 0;
	current_proc->fp_to_sb = -(current_proc->locals_size + current_proc->spills_size);
    current_proc->fp_to_rb = current_proc->fp_to_sb - current_proc->saved_size;
}

struct hop* platform_prologue(void)
{
    int i;
    int spoffset = current_proc->saved_size + current_proc->spills_size +
        current_proc->locals_size;
	struct hop* hop = new_hop(current_proc->entry, NULL);

    hop_add_insel(hop, "! locals_size = %d", current_proc->locals_size);
    hop_add_insel(hop, "! spills_size = %d", current_proc->spills_size);
    hop_add_insel(hop, "! saved_size = %d", current_proc->saved_size);
    hop_add_insel(hop, "! params @ fp+%d", current_proc->fp_to_ab);
    hop_add_insel(hop, "! pc @ fp+2");  
    hop_add_insel(hop, "! fp @ fp+0");  
    hop_add_insel(hop, "! locals @ fp-%d to fp+0",
        current_proc->locals_size);
    hop_add_insel(hop, "! spills @ fp-%d to fp-%d",
        -current_proc->fp_to_sb, current_proc->locals_size);
	hop_add_insel(hop, "push iy");
	hop_add_insel(hop, "ld iy, sp");
	hop_add_insel(hop, "ld hl, %d", -spoffset);
	hop_add_insel(hop, "add hl, sp");
	hop_add_insel(hop, "ld sp, hl");
	return hop;
}

struct hop* platform_epilogue(void)
{
	struct hop* hop = new_hop(current_proc->exit, NULL);
    int i;

	hop_add_insel(hop, "ld sp, iy");
	hop_add_insel(hop, "pop iy");
	hop_add_insel(hop, "ret");

	return hop;
}

struct hop* platform_move(struct basicblock* bb, struct vreg* vreg, struct hreg* src, struct hreg* dest)
{
    struct hop* hop = new_hop(bb, NULL);

    if ((src->attrs & TYPE_ATTRS) != (dest->attrs & TYPE_ATTRS))
            fatal("hreg move of %%%d from %s to %s with mismatched types", vreg->id, src->id, dest->id);
    else
    {
        uint32_t type = src->attrs & TYPE_ATTRS;
		tracef('R', "R: non-converting move from %s to %s of type 0x%x\n", src->id, dest->id, type);

        if (!src->is_stacked && dest->is_stacked)
        {
            switch (type)
            {
                case burm_int_ATTR:
                    hop_add_insel(hop, "sw %H, %S(fp) ! %H", src, dest, dest);
                    break;

                case burm_long_ATTR:
                    hop_add_insel(hop, "sw %0H, 0+%S(fp) ! %H", src, dest, dest);
                    hop_add_insel(hop, "sw %1H, 4+%S(fp) ! %H", src, dest, dest);
                    break;

                case burm_float_ATTR:
                    hop_add_insel(hop, "swc1 %H, %S(fp) ! %H", src, dest, dest);
                    break;

                case burm_double_ATTR:
                    hop_add_insel(hop, "sdc1 %H, %S(fp) ! %H", src, dest, dest);
                    break;

                default:
                    goto nomove;
            }
        }
        else if (src->is_stacked && !dest->is_stacked)
        {
            switch (type)
            {
                case burm_int_ATTR:
                    hop_add_insel(hop, "lw %H, %S(fp) ! %H", dest, src, src);
                    break;

                case burm_long_ATTR:
                    /* Can't load straight into dest because it might overlap with src. */
                    hop_add_insel(hop, "lw at, 0+%S(fp) ! %H", dest, src, src);
                    hop_add_insel(hop, "lw %1H, 4+%S(fp) ! %H", dest, src, src);
                    hop_add_insel(hop, "mov %0H, at", dest);
                    break;

                case burm_float_ATTR:
                    hop_add_insel(hop, "lwc1 %H, %S(fp) ! %H", dest, src, src);
                    break;

                case burm_double_ATTR:
                    hop_add_insel(hop, "ldc1 %H, %S(fp) ! %H", dest, src, src);
                    break;

                default:
                    goto nomove;
            }
        }
        else if (!src->is_stacked && !dest->is_stacked)
        {
            switch (type)
            {
                case burm_int_ATTR:
                    hop_add_insel(hop, "mov %H, %H", dest, src);
                    break;

                case burm_long_ATTR:
                    hop_add_insel(hop, "mov %0H, %0H", dest, src);
                    hop_add_insel(hop, "mov %1H, %1H", dest, src);
                    break;

                case burm_float_ATTR:
                    hop_add_insel(hop, "mov.s %H, %H", dest, src);
                    break;

                case burm_double_ATTR:
                    hop_add_insel(hop, "mov.d %H, %H", dest, src);
                    break;

                default:
                    goto nomove;
            }
        }
        else if (src->is_stacked && dest->is_stacked)
            fatal("tried to move stacked object %%%d of type 0x%x from %s to %s", vreg->id, type, src->id, dest->id);
        else
            goto nomove;
    }

    return hop;

nomove:
    fatal("cannot move %s to %s", src->id, dest->id);
}

struct hop* platform_swap(struct basicblock* bb, struct hreg* src, struct hreg* dest)
{
    struct hop* hop = new_hop(bb, NULL);

	tracef('R', "R: swap of %s to %s\n", src->id, dest->id);
    assert(!src->is_stacked);
    assert(!dest->is_stacked);
    assert((src->attrs & TYPE_ATTRS) == (dest->attrs & TYPE_ATTRS));
    
    switch (src->attrs & TYPE_ATTRS)
    {
        case burm_int_ATTR:
            hop_add_insel(hop, "mov at, %H", src);
            hop_add_insel(hop, "mov %H, %H", src, dest);
            hop_add_insel(hop, "mov %H, at", dest);
            break;

        case burm_long_ATTR:
            hop_add_insel(hop, "mov at, %0H", src);
            hop_add_insel(hop, "mov %0H, %0H", src, dest);
            hop_add_insel(hop, "mov %0H, at", dest);

            hop_add_insel(hop, "mov at, %1H", src);
            hop_add_insel(hop, "mov %1H, %1H", src, dest);
            hop_add_insel(hop, "mov %1H, at", dest);
            break;

        case burm_float_ATTR:
            hop_add_insel(hop, "mov.s f30, %H", src);
            hop_add_insel(hop, "mov.s %H, %H", src, dest);
            hop_add_insel(hop, "mov.s %H, f30", dest);
            break;

        case burm_double_ATTR:
            hop_add_insel(hop, "mov.d f30, %H", src);
            hop_add_insel(hop, "mov.d %H, %H", src, dest);
            hop_add_insel(hop, "mov.d %H, f30", dest);
            break;
    }

    return hop;
}

const char* platform_label(const char* label)
{
    /* Labels starting with . are internal, not exported, and don't need mangling. */

    if (label[0] == '.')
        return label;

    /* Otherwise, mangle. */

    return aprintf("_%s", label);
}

/* vim: set sw=4 ts=4 expandtab : */

