#include "mcg.h"

/* mcg stack frames are laid out as:
 *
 * |    ...params...
 * |  --------------- <- ab
 * |       old FR
 * |       old FP
 * |  --------------- <- fp (a.k.a. lb)
 * |      locals
 * |  ---------------
 * |      spills
 * |  --------------- <- sb
 * |     saved regs
 * |  --------------- <- sp, rb
 * V  ...user area...
 *
 * st indexes up; lb indexes down.
 *
 * Note that [fp] == old_fp and ab == fp + 8.
 */

static ARRAYOF(struct hreg) saved_regs;

void platform_calculate_offsets(void)
{
	int i;

	saved_regs.count = 0;

	current_proc->fp_to_ab = 6;
	current_proc->fp_to_lb = 0;
	current_proc->fp_to_sb
	    = -(current_proc->locals_size + current_proc->spills_size);
	current_proc->fp_to_rb = current_proc->fp_to_sb - current_proc->saved_size;
}

struct hop* platform_prologue(void)
{
	int i;
	int spoffset = current_proc->saved_size + current_proc->spills_size
	    + current_proc->locals_size;
	struct hop* hop = new_hop(current_proc->entry, NULL);

	hop_add_insel(hop, "! locals_size = %d", current_proc->locals_size);
	hop_add_insel(hop, "! spills_size = %d", current_proc->spills_size);
	hop_add_insel(hop, "! saved_size = %d", current_proc->saved_size);
	hop_add_insel(hop, "! params @ fp+%d", current_proc->fp_to_ab);
	hop_add_insel(hop, "! lr @ fp+4");
	hop_add_insel(hop, "! fp @ fp+0");
	hop_add_insel(hop, "! locals @ fp-%d to fp+0", current_proc->locals_size);
	hop_add_insel(
	    hop, "! spills @ fp-%d to fp-%d", -current_proc->fp_to_sb,
	    current_proc->locals_size);
    #if 0
	for (i = saved_regs.count - 1; i >= 0; i--)
	{
		struct hreg* hreg = saved_regs.item[i];
		hop_add_insel(
		    hop, "! %s @ fp-%d", hreg->id,
		    -(current_proc->fp_to_rb + hreg->offset));
	}
    #endif

	hop_add_insel(hop, "push iy");
	hop_add_insel(hop, "ld iy, 0");
	hop_add_insel(hop, "add iy, sp");
	if (spoffset)
	{
		hop_add_insel(hop, "ld hl, -%d", spoffset);
		hop_add_insel(hop, "add hl, sp");
		hop_add_insel(hop, "ld sp, hl");
	}

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

struct hop* platform_load(struct basicblock* bb, struct move* move)
{
	struct hop* hop = new_hop(bb, NULL);

	if ((move->vreg->type & TYPE_ATTRS) != (move->hreg->attrs & TYPE_ATTRS))
		fatal(
		    "hreg load of %%%d in %s with mismatched types", move->vreg->id,
		    move->hreg->id);
	else
	{
		uint32_t type = move->vreg->type & TYPE_ATTRS;
		int offset = move->vreg->congruence->offset;

		assert(!move->other);

		tracef(
		    'R', "R: load stack slot %d into %s for %%%d\n",
		    move->vreg->congruence->offset, move->hreg->id, move->vreg->id);

		switch (type)
		{
			case burm_int_ATTR:
			case burm_float_ATTR:
				hop_add_insel(hop, "ld %H, (iy+%S)", move->hreg, offset);
				break;

			case burm_long_ATTR:
			case burm_double_ATTR:
				hop_add_insel(hop, "exx");
				hop_add_insel(hop, "ld %H, (iy+%S)", move->hreg, offset + 3);
				hop_add_insel(hop, "exx");
				hop_add_insel(hop, "ld %H, (iy+%S)", move->hreg, offset);
				break;

			default:
				fatal(
				    "cannot load %%%d with attrs %x", move->vreg->id,
				    move->vreg->type);
		}
	}

	return hop;
}

struct hop* platform_store(struct basicblock* bb, struct move* move)
{
	struct hop* hop = new_hop(bb, NULL);

	if ((move->vreg->type & TYPE_ATTRS) != (move->hreg->attrs & TYPE_ATTRS))
		fatal(
		    "hreg store of %%%d in %s with mismatched types", move->vreg->id,
		    move->hreg->id);
	else
	{
		uint32_t type = move->vreg->type & TYPE_ATTRS;
		int offset = move->vreg->congruence->offset;

		assert(!move->other);

		tracef(
		    'R', "R: store stack slot %d from %s for %%%d\n",
		    move->vreg->congruence->offset, move->hreg->id, move->vreg->id);

		switch (type)
		{
			case burm_int_ATTR:
			case burm_float_ATTR:
				hop_add_insel(hop, "ld (iy+%S), %H", offset, move->hreg);
				break;

			case burm_long_ATTR:
			case burm_double_ATTR:
				hop_add_insel(hop, "ld (iy+%S), %H", offset, move->hreg);
				hop_add_insel(hop, "exx");
				hop_add_insel(hop, "ld (iy+%S), %H", offset + 3, move->hreg);
				hop_add_insel(hop, "exx");
				break;

			default:
				fatal(
				    "cannot store %%%d with attrs %x", move->vreg->id,
				    move->vreg->type);
		}
	}

	return hop;
}

const char* platform_label(const char* label)
{
	/* Labels starting with . are internal, not exported, and don't need
	 * mangling. */

	if (label[0] == '.')
		return label;

	/* Otherwise, mangle. */

	return aprintf("_%s", label);
}

/* vim: set sw=4 ts=4 expandtab : */
