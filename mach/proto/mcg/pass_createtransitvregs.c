#include "mcg.h"

/* Rewrites all hops which produce or consume values to split live ranges
 * between values in use (i.e. referred to directly by an instruction, and
 * therefore in a register) and values in transit (i.e. not referred to by
 * instructions other than copies, and therefore which don't need to be in
 * registers). */

static void replace_vreg(struct hop* hop, struct vreg* oldvreg, struct vreg* newvreg)
{
	int i;

	if (hop->output == oldvreg)
		hop->output = newvreg;

	for (i=0; i<hop->ins.count; i++)
	{
		if (hop->ins.item[i] == oldvreg)
			hop->ins.item[i] = newvreg;
	}

	for (i=0; i<hop->outs.count; i++)
	{
		if (hop->outs.item[i] == oldvreg)
			hop->outs.item[i] = newvreg;
	}

	for (i=0; i<hop->insels.count; i++)
	{
		struct insel* insel = hop->insels.item[i];
		if ((insel->type == INSEL_VREG) && (insel->u.vreg == oldvreg))
			insel->u.vreg = newvreg;
	}

}

static struct hop* create_copy(struct basicblock* bb, struct vreg* oldvreg, struct vreg* newvreg)
{
	struct hop* hop = new_hop(bb, NULL);
	hop->output = newvreg;
	array_append(&hop->ins, oldvreg);
	array_append(&hop->outs, newvreg);
	hop->is_copy = true;
}

static void rewrite_hops(struct basicblock* bb)
{
	int hopindex;
	int i;

	for (hopindex = 0; hopindex < bb->hops.count; hopindex++)
	{
		struct hop* hop = bb->hops.item[hopindex];
		if (!hop->is_copy)
		{
			for (i=0; i<hop->ins.count; i++)
			{
				struct vreg* oldvreg = hop->ins.item[i];
				struct vreg* newvreg = new_vreg();
				newvreg->type = oldvreg->type;

				replace_vreg(hop, oldvreg, newvreg);

				array_insert(&bb->hops, create_copy(bb, oldvreg, newvreg), hopindex);
				hopindex++;
			}

			for (i=0; i<hop->outs.count; i++)
			{
				struct vreg* oldvreg = hop->outs.item[i];
				struct vreg* newvreg = new_vreg();
				newvreg->type = oldvreg->type;

				replace_vreg(hop, oldvreg, newvreg);

				array_insert(&bb->hops, create_copy(bb, newvreg, oldvreg), hopindex+1);
			}
		}
	}
}

void pass_create_transit_vregs(void)
{
	int i;

	for (i = 0; i < current_proc->blocks.count; i++)
	{
		struct basicblock* bb = current_proc->blocks.item[i];
		rewrite_hops(bb);
	}
}
