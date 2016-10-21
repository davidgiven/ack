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

static ARRAYOF(struct hreg) saved_regs;

void platform_calculate_offsets(void)
{
    int i;

    saved_regs.count = 0;
    for (i=0; i<current_proc->usedregs.count; i++)
    {
        struct hreg* hreg = current_proc->usedregs.item[i];

        if (!(hreg->attrs & burm_volatile_ATTR) &&
            ((hreg->attrs & burm_int_ATTR) || (hreg->attrs & burm_float_ATTR)))
        {
            current_proc->saved_size += 4;
            array_append(&saved_regs, hreg);
        }
    }

	current_proc->fp_to_st = 0;
	current_proc->fp_to_ab = current_proc->spills_size + current_proc->saved_size + 8;
	current_proc->fp_to_lb = 0;
}

struct hop* platform_prologue(void)
{
    int i;
    int saved_offset;
	struct hop* hop = new_hop(current_proc->entry, NULL);

	hop_add_insel(hop, "! saved_size = %d+8 bytes", current_proc->saved_size);
	hop_add_insel(hop, "! spills_size = %d bytes", current_proc->spills_size);
	hop_add_insel(hop, "! locals_size = %d bytes", current_proc->locals_size);
	hop_add_insel(hop, "addi sp, sp, %d", -(current_proc->fp_to_ab + current_proc->locals_size));
	hop_add_insel(hop, "mfspr r0, lr");

	hop_add_insel(hop, "stw r0, %d(sp)", current_proc->locals_size + current_proc->spills_size);
	hop_add_insel(hop, "stw fp, %d(sp)", current_proc->locals_size + current_proc->spills_size + 4);
	hop_add_insel(hop, "addi fp, sp, %d", current_proc->locals_size);

    saved_offset = current_proc->spills_size + 8;
    for (i=0; i<saved_regs.count; i++)
    {
        struct hreg* hreg = saved_regs.item[i];
        if (hreg->type & burm_int_ATTR)
            hop_add_insel(hop, "stw %H, %d(fp)", hreg, saved_offset);
        else if (hreg->type & burm_float_ATTR)
            hop_add_insel(hop, "stfs %H, %d(fp)", hreg, saved_offset);
        saved_offset += 4;
    }
	return hop;
}

struct hop* platform_epilogue(void)
{
	struct hop* hop = new_hop(current_proc->exit, NULL);
    int i;
    int saved_offset;

    saved_offset = current_proc->spills_size + 8;
    for (i=0; i<saved_regs.count; i++)
    {
        struct hreg* hreg = saved_regs.item[i];
        if (hreg->type & burm_int_ATTR)
            hop_add_insel(hop, "lwz %H, %d(fp)", hreg, saved_offset);
        else if (hreg->type & burm_float_ATTR)
            hop_add_insel(hop, "lfs %H, %d(fp)", hreg, saved_offset);
        saved_offset += 4;
    }

    hop_add_insel(hop, "lwz r0, %d(fp)", current_proc->spills_size);
    hop_add_insel(hop, "mtspr lr, r0");
    hop_add_insel(hop, "lwz r0, %d(fp)", current_proc->spills_size + 4);
    hop_add_insel(hop, "addi sp, fp, %d", current_proc->fp_to_ab);
    hop_add_insel(hop, "mr fp, r0");
    hop_add_insel(hop, "bclr 20, 0, 0");

	return hop;
}

struct hop* platform_move(struct basicblock* bb, struct hreg* src, struct hreg* dest)
{
    struct hop* hop = new_hop(bb, NULL);
    const uint32_t type_attrs =
        burm_int_ATTR | burm_pair_ATTR | burm_float_ATTR | burm_double_ATTR;

    if ((src->type & type_attrs) != (dest->type & type_attrs))
    {
        assert(!src->is_stacked);
        assert(!dest->is_stacked);

        switch (src->type & type_attrs)
        {
            case burm_int_ATTR:
                hop_add_insel(hop, "stwu %H, -4(sp)", src);
                break;

            case burm_float_ATTR:
                hop_add_insel(hop, "stfsu %H, -4(sp)", src);
                break;

            case burm_double_ATTR:
                hop_add_insel(hop, "stfdu %H, -8(sp)", src);
                break;

            default:
                assert(false);
        }

        switch (dest->type & type_attrs)
        {
            case burm_int_ATTR:
                hop_add_insel(hop, "lwz %H, 0(sp)", dest);
                break;

            case burm_float_ATTR:
                hop_add_insel(hop, "lfs %H, 0(sp)", dest);
                break;

            case burm_double_ATTR:
                hop_add_insel(hop, "lfd %H, 0(sp)", dest);
                break;

            default:
                assert(false);
        }

        switch (dest->type & type_attrs)
        {
            case burm_int_ATTR:
            case burm_float_ATTR:
                hop_add_insel(hop, "addi sp, sp, 4");
                break;

            case burm_double_ATTR:
            case burm_pair_ATTR:
                hop_add_insel(hop, "addi sp, sp, 8");
                break;

            default:
                assert(false);
        }
    }
    else
    {
        uint32_t type = src->type & type_attrs;

        if (!src->is_stacked && dest->is_stacked)
        {
            switch (type)
            {
                case burm_int_ATTR:
                    hop_add_insel(hop, "stw %H, %S(fp) ! %H", src, dest, dest);
                    break;

                case burm_float_ATTR:
                    hop_add_insel(hop, "stfs %H, %S(fp) ! %H", src, dest, dest);
                    break;

                default:
                    assert(false);
            }
        }
        else if (src->is_stacked && !dest->is_stacked)
        {
            switch (type)
            {
                case burm_int_ATTR:
                    hop_add_insel(hop, "lwz %H, %S(fp) ! %H", dest, src, src);
                    break;

                case burm_float_ATTR:
                    hop_add_insel(hop, "lfs %H, %S(fp) ! %H", dest, src, src);
                    break;

                default:
                    assert(false);
            }
        }
        else if (!src->is_stacked && !dest->is_stacked)
        {
            switch (type)
            {
                case burm_int_ATTR:
                    hop_add_insel(hop, "mr %H, %H", dest, src);
                    break;

                case burm_pair_ATTR:
                    hop_add_insel(hop, "mr %0H, %0H", dest, src);
                    hop_add_insel(hop, "mr %1H, %1H", dest, src);
                    break;

                case burm_float_ATTR:
                case burm_double_ATTR:
                    hop_add_insel(hop, "fmr %H, %H", dest, src);
                    break;

                default:
                    assert(false);
            }
        }
        else
            assert(false);
    }

    return hop;
}

/* vim: set sw=4 ts=4 expandtab : */

