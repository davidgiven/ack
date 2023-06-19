#include "mcg.h"

static ARRAYOF(struct hreg) hregs;

static struct move* create_move(struct vreg* vreg)
{
	struct move* m = calloc(1, sizeof(struct move));
	m->vreg = vreg;
	return m;
}

static void populate_hregs(void)
{
	int i;
	const struct burm_register_data* brd = burm_register_data;

	hregs.count = 0;
	while (brd->id)
	{
		array_append(&hregs, new_hreg(brd));
		brd++;
	}

	/* Wire up the register aliases. */

	for (i = 0; i < hregs.count; i++)
	{
		struct hreg* hreg = hregs.item[i];
		const struct burm_register_data** alias = hreg->brd->aliases;

		while (*alias)
		{
			int index = *alias - burm_register_data;
			hreg->usage |= (1 << index);
			alias++;
		}

		tracef(
		    'R', "R: found hreg %s, type %08x, aliases %08x\n", hreg->id,
		    hreg->attrs, hreg->usage);
	}
}

static void assign_hop_producer_consumers(void)
{
	int i, j, k;

	for (i = 0; i < dominance.preorder.count; i++)
	{
		struct basicblock* bb = dominance.preorder.item[i];

		for (j = 0; j < bb->hops.count; j++)
		{
			struct hop* hop = bb->hops.item[j];

			for (k = 0; k < hop->ins.count; k++)
			{
				struct vreg* vreg = hop->ins.item[k];
				struct move* m = create_move(vreg);
				array_append(&hop->consumes, m);
			}

			for (k = 0; k < hop->outs.count; k++)
			{
				struct vreg* vreg = hop->outs.item[k];
				struct move* m = create_move(vreg);
				array_append(&hop->produces, m);
			}
		}
	}
}

static int
pack_hreg(int stacksize, int size, uint32_t attr, struct congruence* g)
{
	if ((g->type & attr) && (g->offset == -1))
	{
		g->offset = stacksize;
		stacksize += size;
	}

	return stacksize;
}

static int pack_stackframe(int stacksize, int size, uint32_t attr)
{
	int i, j, k;

	for (i = 0; i < dominance.preorder.count; i++)
	{
		struct basicblock* bb = dominance.preorder.item[i];

		for (j = 0; j < bb->hops.count; j++)
		{
			struct hop* hop = bb->hops.item[j];

			for (k = 0; k < hop->produces.count; k++)
			{
				struct move* m = hop->produces.item[k];
				if (m->spilled)
					stacksize
					    = pack_hreg(stacksize, size, attr, m->vreg->congruence);
			}

			for (k = 0; k < hop->consumes.count; k++)
			{
				struct move* m = hop->consumes.item[k];
				if (m->spilled)
					stacksize
					    = pack_hreg(stacksize, size, attr, m->vreg->congruence);
			}
		}
	}

	return stacksize;
}

static void layout_stack_frame(void)
{
	int stacksize = 0;
	stacksize = pack_stackframe(stacksize, EM_wordsize * 2, burm_double_ATTR);
	stacksize = pack_stackframe(stacksize, EM_wordsize * 2, burm_long_ATTR);
	stacksize = pack_stackframe(stacksize, EM_wordsize * 1, burm_float_ATTR);
	stacksize = pack_stackframe(stacksize, EM_wordsize * 1, burm_int_ATTR);
	current_proc->spills_size = stacksize;
	tracef('R', "R: spill size is %d bytes\n", stacksize);
}

static bool register_available(uint32_t used, struct hreg* hreg)
{
	return (used & hreg->usage) == 0;
}

static struct hreg* allocate_register(uint32_t used, uint32_t type)
{
	int i;

	/* Find an unused register of the appropriate type. */

	for (i = 0; i < hregs.count; i++)
	{
		struct hreg* hreg = hregs.item[i];
		if (register_available(used, hreg) && (hreg->attrs & type))
		{
			/* This one is unused. Use it. */

			return hreg;
		}
	}

	fatal("failed to allocate register");
}

static struct constraint* get_constraint(struct hop* hop, struct vreg* vreg)
{
	return pmap_findleft(&hop->constraints, vreg);
}

static struct move* find_move(struct move** ptr, int count, struct vreg* vreg)
{
	while (count--)
	{
		struct move* m = *ptr++;
		if (m->vreg == vreg)
			return m;
	}

	return NULL;
}

static void allocate_remaining_registers(void)
{
	int i, j, k;

	for (i = 0; i < dominance.preorder.count; i++)
	{
		struct basicblock* bb = dominance.preorder.item[i];

		for (j = 0; j < bb->hops.count; j++)
		{
			struct hop* hop = bb->hops.item[j];

			for (k = 0; k < hop->produces.count; k++)
			{
				struct move* m = hop->produces.item[k];
				if (!m->hreg)
				{
					struct hreg* hreg;
					struct constraint* constraint
					    = get_constraint(hop, m->vreg);
					uint32_t usagemask = hop->outputregusage;

					/* For an equals-to constraint, we need to find a matching
					 * input register. */

					if (constraint && constraint->equals_to)
						usagemask |= hop->inputregusage;

					hreg = allocate_register(hop->inputregusage, m->vreg->type);
					pmap_put(&hop->assignments, m->vreg, hreg);
					tracef(
					    'R', "R: allocate output register for %%%d g%d: %s\n",
					    m->vreg->id, m->vreg->congruence->id, hreg->id);
					hop->outputregusage |= hreg->usage;

					if (constraint && constraint->equals_to)
					{
						struct move* a = find_move(
						    hop->consumes.item, hop->consumes.count,
						    constraint->equals_to);
						assert(a != NULL);
						assert(a->hreg == NULL);
						a->hreg = hreg;
						a->spilled = true;
						hop->inputregusage |= hreg->usage;
						pmap_put(&hop->assignments, a->vreg, hreg);
						tracef(
						    'R', "R: also input register for %%%d g%d: %s\n",
						    a->vreg->id, a->vreg->congruence->id, hreg->id);
					}

					m->spilled = true;
				}
			}

			for (k = 0; k < hop->consumes.count; k++)
			{
				struct move* m = hop->consumes.item[k];
				if (!m->hreg)
				{
					struct hreg* hreg;
					struct constraint* constraint
					    = get_constraint(hop, m->vreg);
					uint32_t usagemask = hop->inputregusage;

					/* For a preserved constraint, we need to find a matching
					 * output register. */

					if (constraint && constraint->preserved)
						usagemask |= hop->outputregusage;

					hreg = allocate_register(hop->inputregusage, m->vreg->type);
					hop->inputregusage |= hreg->usage;
					pmap_put(&hop->assignments, m->vreg, hreg);
					tracef(
					    'R', "R: allocate input register for %%%d g%d: %s\n",
					    m->vreg->id, m->vreg->congruence->id, hreg->id);

					if (constraint && constraint->preserved)
						hop->outputregusage |= hreg->usage;

					m->spilled = true;
				}
			}
		}
	}
}

void pass_register_allocator(void)
{
	populate_hregs();
	assign_hop_producer_consumers();
	allocate_remaining_registers();
	layout_stack_frame();
}

/* vim: set sw=4 ts=4 expandtab : */
