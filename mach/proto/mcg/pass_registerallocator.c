#include "mcg.h"

struct interval
{
	int id;
	int inclusiveStart;
	int exclusiveEnd;
	struct congruence* congruence;
};

static ARRAYOF(struct hop) hops;
static PMAPOF(struct congruence, struct interval) intervals;
static PMAPOF(struct congruence, struct interval) active;

static struct interval* create_interval(void)
{
	static int number = 0;
	struct interval* v = calloc(1, sizeof(struct interval*));
	v->id = number++;
	return v;
}

static void collect_hops(void)
{
	int i, j;
	hops.count = 0;

	for (i = 0; i < dominance.preorder.count; i++)
	{
		struct basicblock* bb = dominance.preorder.item[i];
		array_appendall(&hops, &bb->hops);
	}
	tracef('R', "R: function has %d hops\n", hops.count);
}

static void collect_intervals(void)
{
	int i, j;
	static ARRAYOF(struct congruence) groups;

	active.count = 0;
	for (i = 0; i < hops.count; i++)
	{
		struct hop* hop = hops.item[i];

		groups.count = 0;
		for (j = 0; j < hop->ins.count; j++)
			array_appendu(&groups, hop->ins.item[j]->congruence);
		for (j = 0; j < hop->outs.count; j++)
			array_appendu(&groups, hop->outs.item[j]->congruence);
		for (j = 0; j < hop->throughs.count; j++)
			array_appendu(&groups, hop->throughs.item[j]->congruence);

		for (j = 0; j < groups.count; j++)
		{
			struct congruence* g = groups.item[j];

			/* For any congruence group used in this hop which isn't in the
			 * active set, then this marks the start of an interval. */

			if (!pmap_findleft(&active, g))
			{
				struct interval* v = create_interval();
				v->inclusiveStart = i;
				v->congruence = g;
				pmap_add(&active, g, v);
				pmap_add(&intervals, g, v);
			}
		}

		for (j = 0; j < active.count; j++)
		{
			struct congruence* g = active.item[j].left;
			struct interval* v = active.item[j].right;

			/* For any congruence group which isn't used in this hop but which
			 * _is_ in the active set, then this must be the end of an
			 * interval. */

			if (!array_contains(&groups, g))
			{
				v->exclusiveEnd = i;
				pmap_remove(&active, g, v);
			}
		}
	}

	for (i = 0; i < intervals.count; i++)
	{
		struct interval* v = intervals.item[i].right;
		tracef(
		    'R', "R: interval %d for group %d from hop %d to %d\n", v->id,
		    v->congruence->id, v->inclusiveStart, v->exclusiveEnd);
	}
}

void pass_register_allocator(void)
{
	collect_hops();
	collect_intervals();
}

/* vim: set sw=4 ts=4 expandtab : */
