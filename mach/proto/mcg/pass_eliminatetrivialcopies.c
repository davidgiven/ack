#include "mcg.h"

static void
rewrite_hop(struct hop* hop, struct vreg* invreg, struct vreg* outvreg)
{
	int j;

	tracef('C', "C: rewriting hop $%d\n", hop->id);

	/* Rewire the hop with uses invreg. */

	for (j = 0; j < hop->insels.count; j++)
	{
		struct insel* insel = hop->insels.item[j];
		if ((insel->type == INSEL_VREG) && (insel->u.vreg == invreg))
			insel->u.vreg = outvreg;
	}

	if (hop->output == invreg)
		hop->output = outvreg;

	for (j = 0; j < hop->constraints.count; j++)
	{

		if (hop->constraints.item[j].left == invreg)
			hop->constraints.item[j].left = outvreg;

		if (hop->constraints.item[j].right->equals_to == invreg)
			hop->constraints.item[j].right->equals_to = outvreg;
	}

	for (j = 0; j < hop->ins.count; j++)
		if (hop->ins.item[j] == invreg)
			hop->ins.item[j] = outvreg;

	for (j = 0; j < hop->outs.count; j++)
		if (hop->outs.item[j] == invreg)
			hop->outs.item[j] = outvreg;

    assert(hop->throughs.count == 0);

	array_appendu(&outvreg->used, hop);

	/* Rewire the bb which the hop is inside. */

	for (j = 0; j < hop->bb->phis.count; j++)
	{
		if (hop->bb->phis.item[j].left == invreg)
			hop->bb->phis.item[j].left = outvreg;
	}
}

static bool check_hop(struct hop* hop)
{
	int i, j;

	if ((hop->insels.count != 0) || (hop->ins.count != 1)
	    || (hop->outs.count != 1))
		return false;

	struct vreg* invreg = hop->ins.item[0];
	struct vreg* outvreg = hop->outs.item[0];
	if (invreg == outvreg)
		return false;
	if (invreg->type != outvreg->type)
		return false;

	tracef(
	    'C', "C: found trivial copy of %%%d -> %%%d in hop $%d\n", invreg->id,
	    outvreg->id, hop->id);

	/* For every usage of invreg, rewrite it to outvreg. */

	tracef('C', "C: %%%d has %d uses\n", invreg->id, invreg->used.count);
	for (i = 0; i < invreg->used.count; i++)
		rewrite_hop(invreg->used.item[i], invreg, outvreg);
	rewrite_hop(hop, invreg, outvreg);
    rewrite_hop(invreg->defined, invreg, outvreg);

    array_remove(&hop->ins, outvreg);
    array_remove(&hop->outs, outvreg);

	outvreg->defined = invreg->defined;
	return true;
}

static void eliminate_copies(struct basicblock* bb)
{
	int i;
	bool found;

	do
	{
		found = false;
		for (i = 0; i < bb->hops.count; i++)
			found |= check_hop(bb->hops.item[i]);
	} while (found);
}

void pass_eliminate_trivial_copies(void)
{
	int i;

	for (i = 0; i < current_proc->blocks.count; i++)
	{
		struct basicblock* bb = current_proc->blocks.item[i];
		eliminate_copies(bb);
	}
}

/* vim: set sw=4 ts=4 expandtab : */
