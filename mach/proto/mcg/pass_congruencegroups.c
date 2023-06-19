#include "mcg.h"

static ARRAYOF(struct congruence) groups;

static void update_vreg_types(struct congruence* c)
{
	int i;

	for (i = 0; i < c->vregs.count; i++)
	{
		struct vreg* vreg = c->vregs.item[i];

		if (vreg->type == 0)
			vreg->type = c->type;
		assert(vreg->type == c->type);
		assert(vreg->type != 0);
	}
}

static struct congruence* create_congruence(struct vreg* member)
{
	static int number = 0;

	struct congruence* g = calloc(1, sizeof(struct congruence));
	g->id = number++;
    g->offset = -1;
	array_append(&groups, g);

	array_append(&g->vregs, member);
	g->type = member->type;
	member->congruence = g;

	return g;
}

static void coalesce(struct vreg* reg1, struct vreg* reg2)
{
	struct congruence* g;
	if (!reg1->congruence && !reg2->congruence)
	{
		g = create_congruence(reg1);
		array_appendu(&g->vregs, reg2);
	}
	else if (reg1->congruence && !reg2->congruence)
	{
		g = reg1->congruence;
		array_appendu(&g->vregs, reg2);
	}
	else if (!reg1->congruence && reg2->congruence)
	{
		g = reg2->congruence;
		array_appendu(&g->vregs, reg1);
	}
	else if (reg1->congruence && (reg1->congruence == reg2->congruence))
		return;
	else
	{
		int i;
		struct congruence* og = reg2->congruence;
		g = reg1->congruence;
		if (g->type != og->type)
			fatal(
			    "congruence group with mismatched register types %x and %x",
			    g->type, og->type);

		tracef(
		    'C', "C: coalescing congruence groups %d and %d\n", g->id, og->id);
		for (i = 0; i < og->vregs.count; i++)
		{
			struct vreg* vreg = og->vregs.item[i];
			array_appendu(&g->vregs, vreg);
			vreg->congruence = g;
		}

		og->vregs.count = 0;
	}

	reg1->congruence = reg2->congruence = g;
}

static void process(struct basicblock* bb)
{
	int i;

	/* Coalesce the phi output registers with their sources. */

	for (i = 0; i < bb->phis.count; i++)
	{
		struct vreg* outputvreg = bb->phis.item[i].left;
		struct vreg* inputvreg = bb->phis.item[i].right->ir->result;

		coalesce(inputvreg, outputvreg);
	}

	/* Now go through the hops looking for hops with an equality contraint on
	 * their result register. */

	for (i = 0; i < bb->hops.count; i++)
	{
		struct hop* hop = bb->hops.item[i];
		struct vreg* output = hop->output;
		if (output)
		{
			struct constraint* c = pmap_findleft(&hop->constraints, output);
			if (c && c->equals_to)
				coalesce(output, c->equals_to);

			if (!output->congruence)
				create_congruence(output);
		}
	}
}

void pass_find_congruence_groups(void)
{
	int i, j;

	groups.count = 0;

	for (i = 0; i < dominance.preorder.count; i++)
		process(dominance.preorder.item[i]);

	for (i = 0; i < groups.count; i++)
	{
		struct congruence* group = groups.item[i];
		int type = 0;

		for (j = 0; j < group->vregs.count; j++)
		{
			struct vreg* vreg = group->vregs.item[j];
			if (!vreg->type)
				vreg->type = group->type;
		}

		tracef(
		    'C', "C: congruence group %d of type %x is:", group->id,
		    group->type);
		for (j = 0; j < group->vregs.count; j++)
			tracef('C', " %%%d", group->vregs.item[j]->id);
		tracef('C', "\n");
	}
}

/* vim: set sw=4 ts=4 expandtab : */
