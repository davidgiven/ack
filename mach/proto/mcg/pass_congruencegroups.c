#include "mcg.h"

static ARRAYOF(struct congruence) groups;

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
        if (!g->type)
            g->type = reg2->type;
	}
	else if (reg1->congruence && !reg2->congruence)
	{
		g = reg1->congruence;
		array_appendu(&g->vregs, reg2);
        if (!g->type)
            g->type = reg2->type;
	}
	else if (!reg1->congruence && reg2->congruence)
	{
		g = reg2->congruence;
		array_appendu(&g->vregs, reg1);
        if (!g->type)
            g->type = reg1->type;
	}
	else if (reg1->congruence && (reg1->congruence == reg2->congruence))
		return;
	else
	{
		int i;
		struct congruence* og = reg2->congruence;
		g = reg1->congruence;

        if (!g->type)
            g->type = og->type;
        if (!og->type)
            og->type = g->type;
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

    /* Ensure that any cross-bb vregs which _aren't_ used in a phi are also congruenced. */

    for (i = 0; i < bb->liveouts.count; i++)
    {
        struct vreg* vreg = bb->liveouts.item[i];
        if (!vreg->congruence)
            create_congruence(vreg);
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

		if (group->vregs.count != 0)
		{
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
}

/* vim: set sw=4 ts=4 expandtab : */
