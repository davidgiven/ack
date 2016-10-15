#include "mcg.h"

static int vreg_count = 1;

struct vreg* new_vreg(void)
{
	struct vreg* vreg = calloc(1, sizeof *vreg);
	vreg->id = vreg_count++;
	return vreg;
}

struct hreg* new_hreg(const struct burm_register_data* brd)
{
	struct hreg* hreg = calloc(1, sizeof *hreg);
	hreg->name = brd->name;
    hreg->realname = brd->realname;
    hreg->type = brd->type;
	hreg->attrs = brd->attrs;
	hreg->is_stacked = false;
	return hreg;
}

struct hreg* new_stacked_hreg(int offset, uint32_t type)
{
	struct hreg* hreg = calloc(1, sizeof *hreg);
	hreg->name = aprintf("stacked_%d", offset);
    hreg->realname = hreg->name;
    hreg->type = type;
	hreg->attrs = type;
	hreg->is_stacked = true;
	hreg->offset = offset;
	return hreg;
}

/* vim: set sw=4 ts=4 expandtab : */

