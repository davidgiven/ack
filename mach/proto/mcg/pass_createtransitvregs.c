#include "mcg.h"

/* Rewrites all hops which produce or consume values to split live ranges
 * between values in use (i.e. referred to directly by an instruction, and
 * therefore in a register) and values in transit (i.e. not referred to by
 * instructions other than copies, and therefore which don't need to be in
 * registers). */

static void
replace_vreg(struct hop* hop, struct vreg* oldvreg, struct vreg* newvreg)
{
	int i;

	if (hop->output == oldvreg)
		hop->output = newvreg;

	for (i = 0; i < hop->ins.count; i++)
	{
		if (hop->ins.item[i] == oldvreg)
			hop->ins.item[i] = newvreg;
	}

	for (i = 0; i < hop->outs.count; i++)
	{
		if (hop->outs.item[i] == oldvreg)
			hop->outs.item[i] = newvreg;
	}

	for (i = 0; i < hop->insels.count; i++)
	{
		struct insel* insel = hop->insels.item[i];
		if ((insel->type == INSEL_VREG) && (insel->u.vreg == oldvreg))
			insel->u.vreg = newvreg;
	}

	for (i = 0; i < hop->constraints.count; i++)
	{
		if (hop->constraints.item[i].left == oldvreg)
			hop->constraints.item[i].left = newvreg;
		if (hop->constraints.item[i].right->equals_to == oldvreg)
			hop->constraints.item[i].right->equals_to = newvreg;
	}
}

static struct hop*
create_copy(struct basicblock* bb, struct vreg* oldvreg, struct vreg* newvreg)
{
	struct hop* hop = new_hop(bb, NULL);
	hop->output = newvreg;
	array_append(&hop->ins, oldvreg);
	get_constraint(hop, oldvreg)->regclass = oldvreg->regclass;
	array_append(&hop->outs, newvreg);
	hop->is_copy = true;
}

static void rewrite_hops(struct basicblock* bb)
{
	int i, j, k;

	for (j = 0; j < bb->hops.count; j++)
	{
		struct hop* hop = bb->hops.item[j];
		if (!hop->is_copy)
		{
			for (i = 0; i < hop->ins.count; i++)
			{
				struct vreg* oldvreg = hop->ins.item[i];
				struct vreg* newvreg = new_vreg();
				struct constraint* constraint = get_constraint(hop, oldvreg);
				newvreg->regclass = oldvreg->regclass;
				oldvreg->regclass = constraint->regclass;
				oldvreg->in_transit = true;

				replace_vreg(hop, oldvreg, newvreg);

				array_insert(&bb->hops, create_copy(bb, oldvreg, newvreg), j);
				j++;
			}

			for (i = 0; i < hop->outs.count; i++)
			{
				struct vreg* oldvreg = hop->outs.item[i];
				struct vreg* newvreg = new_vreg();
				newvreg->regclass = oldvreg->regclass;
				oldvreg->in_transit = true;

				replace_vreg(hop, oldvreg, newvreg);

				array_insert(
				    &bb->hops, create_copy(bb, newvreg, oldvreg), j + 1);
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
