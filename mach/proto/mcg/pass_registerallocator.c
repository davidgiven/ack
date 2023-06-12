#include "mcg.h"

struct interval
{
    ARRAYOF(struct vreg) vregs;
};

static PMAPOF(struct vreg, struct interval) vregToInterval;
static ARRAYOF(struct ir) phis;

void pass_register_allocator(void)
{
}

/* vim: set sw=4 ts=4 expandtab : */

