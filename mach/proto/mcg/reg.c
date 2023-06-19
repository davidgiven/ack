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
	hreg->id = brd->id;
    hreg->brd = brd;
	hreg->attrs = brd->attrs;
	hreg->is_stacked = false;
    /* The aliases array needs to be initialised later. */
	return hreg;
}

/* vim: set sw=4 ts=4 expandtab : */

