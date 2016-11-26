#include "mcg.h"

static int vreg_count = 1;

struct vreg* new_vreg(void)
{
	struct vreg* vreg = heap_alloc(&proc_heap, 1, sizeof *vreg);
	vreg->id = vreg_count++;
	return vreg;
}

struct hreg* new_hreg(const struct burm_register_data* brd)
{
	struct hreg* hreg = heap_alloc(&proc_heap, 1, sizeof *hreg);
	hreg->id = brd->id;
    hreg->brd = brd;
	hreg->attrs = brd->attrs;
	hreg->is_stacked = false;
    /* The aliases array needs to be initialised later. */
	return hreg;
}

struct hreg* new_stacked_hreg(uint32_t attrs)
{
    static int hreg_count = 1;
	struct hreg* hreg = heap_alloc(&proc_heap, 1, sizeof *hreg);
	hreg->id = aprintf("stacked_%d_id_%d", attrs, hreg_count++);
	hreg->attrs = attrs;
	hreg->is_stacked = true;
	hreg->offset = -1;
    array_append(&hreg->aliases, hreg);
	return hreg;
}

/* vim: set sw=4 ts=4 expandtab : */

