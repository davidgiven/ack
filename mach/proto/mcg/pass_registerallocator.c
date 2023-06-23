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
				if (!m->other)
					stacksize
					    = pack_hreg(stacksize, size, attr, m->vreg->congruence);
			}

			for (k = 0; k < hop->consumes.count; k++)
			{
				struct move* m = hop->consumes.item[k];
				if (!m->other)
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

static struct hreg* allocate_register(uint32_t used, uint32_t attrs)
{
	int i;

	/* Find an unused register of the appropriate type. */

	for (i = 0; i < hregs.count; i++)
	{
		struct hreg* hreg = hregs.item[i];
		if (!(hreg->usage & used) && ((hreg->attrs & attrs) == attrs))
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
            tracef('R', "R: considering hop %d\n", hop->id);

			for (k = 0; k < hop->produces.count; k++)
			{
				struct move* m = hop->produces.item[k];
				if (!m->hreg)
				{
					struct constraint* constraint
					    = get_constraint(hop, m->vreg);
					uint32_t usagemask = hop->outputregusage;

					assert(!m->other);
					assert(constraint);

					/* For an equals-to constraint, we need to find a matching
					 * input register. */

					if (constraint->equals_to)
						usagemask |= hop->inputregusage;

					m->hreg = allocate_register(
					    hop->inputregusage, constraint->attrs);
					pmap_put(&hop->assignments, m->vreg, m->hreg);
					tracef(
					    'R',
					    "R: allocate output register for %%%d g%d attrs %x: "
					    "%s\n",
					    m->vreg->id, m->vreg->congruence->id, constraint->attrs,
					    m->hreg->id);
					hop->outputregusage |= m->hreg->usage;

					if (constraint->equals_to)
					{
						struct move* a = find_move(
						    hop->consumes.item, hop->consumes.count,
						    constraint->equals_to);

						assert(a != NULL);
						assert(a->hreg == NULL);

						a->hreg = m->hreg;
                        assert(!(hop->inputregusage & m->hreg->usage));
						hop->inputregusage |= m->hreg->usage;
						pmap_put(&hop->assignments, a->vreg, m->hreg);
						tracef(
						    'R', "R: also input register for %%%d g%d: %s\n",
						    a->vreg->id, a->vreg->congruence->id, m->hreg->id);
					}
				}
			}

			for (k = 0; k < hop->consumes.count; k++)
			{
				struct move* m = hop->consumes.item[k];
				if (!m->hreg)
				{
					struct constraint* constraint
					    = get_constraint(hop, m->vreg);
					uint32_t usagemask = hop->inputregusage;

					assert(!m->other);
					assert(constraint);

					/* For a preserved constraint, we need to find a matching
					 * output register. */

					if (constraint->preserved)
						usagemask |= hop->outputregusage;

					m->hreg = allocate_register(usagemask, constraint->attrs);
					hop->inputregusage |= m->hreg->usage;
					pmap_put(&hop->assignments, m->vreg, m->hreg);
					tracef(
					    'R',
					    "R: allocate input register for %%%d g%d attrs %x usage %x from set %x: "
					    "%s\n",
					    m->vreg->id, m->vreg->congruence->id, constraint->attrs,
					    m->hreg->usage, usagemask, m->hreg->id);

					if (constraint->preserved)
                    {
                        assert(!(hop->outputregusage & m->hreg->usage));
						hop->outputregusage |= m->hreg->usage;
                    }
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
