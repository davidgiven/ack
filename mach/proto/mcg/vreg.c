#include "mcg.h"

static int vreg_count = 1;

struct vreg* new_vreg(void)
{
	struct vreg* vreg = calloc(1, sizeof *vreg);
	vreg->id = vreg_count++;
	return vreg;
}
