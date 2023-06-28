#include "mcg.h"

static ARRAYOF(struct hreg) hregs;
static struct bitset interference_graph;
static ARRAYOF(struct hop) current_hops;

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

static void mark_vreg_interference(struct vreg* v1, struct vreg* v2)
{
	bitset_set(&interference_graph, v1->id * vreg_count + v2->id, true);
	bitset_set(&interference_graph, v2->id * vreg_count + v1->id, true);
}

static void construct_interference_graph(void)
{
	int i, j, k, l;

	tracef(
	    'R', "R: creating interference graph of %dx%d vregs\n", vreg_count,
	    vreg_count);
	bitset_resize(&interference_graph, vreg_count * vreg_count);

	for (i = 0; i < dominance.preorder.count; i++)
	{
		struct basicblock* bb = dominance.preorder.item[i];

		for (j = 0; j < bb->hops.count; j++)
		{
			struct hop* hop = bb->hops.item[j];
			static ARRAYOF(struct vreg) vregs;
			vregs.count = 0;

			for (k = 0; k < hop->ins.count; k++)
				array_appendu(&vregs, hop->ins.item[k]);
			for (k = 0; k < hop->outs.count; k++)
				array_appendu(&vregs, hop->outs.item[k]);
			for (k = 0; k < hop->throughs.count; k++)
				array_appendu(&vregs, hop->throughs.item[k]);

			for (k = 0; k < vregs.count; k++)
			{
				struct vreg* v1 = vregs.item[k];
				for (l = k + 1; l < vregs.count; l++)
				{
					struct vreg* v2 = vregs.item[l];
					mark_vreg_interference(v1, v2);
				}
			}
		}
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

static struct hreg* allocate_register(regmask_t used, uint32_t attrs)
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

	return NULL;
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

/* Returns false if the range spans a basic block. */
static bool collect_hops_for_vreg(struct basicblock* bb, struct vreg* vreg)
{
	struct constraint* constraint;
	struct hop* hop;
	int i;
	struct hop* lasthop = vreg->used.item[vreg->used.count - 1];
	struct vreg* nextvreg;

	/* Find the location of hop where this vreg is defined. */

	i = -1;
	do
	{
		i++;
		if (i == bb->hops.count)
			return false;

		hop = bb->hops.item[i];
	} while (hop != vreg->defined);
	if (hop == lasthop)
		return false;
	tracef(
	    'R', "R: vreg %%%d is defined at hop %d, ends at hop %d, index %d\n",
	    vreg->id, hop->id, lasthop->id, i);

	/* Copy hops until we reach the last one. */

	for (;;)
	{
		array_append(&current_hops, hop);
		hop->vreg_being_allocated = vreg;

		if (hop == lasthop)
			break;

		i++;
		if (i == bb->hops.count)
			return false;
		hop = bb->hops.item[i];

		if (hop->bb != bb)
			return false;
	}

	return true;
}

static regmask_t compute_allocation_bitmap(int startincl, int endincl)
{
	struct hop* hop = current_hops.item[0];
	struct vreg* vreg = NULL;
	struct constraint* constraint;
	regmask_t mask = 0;
	int i;

	constraint = get_constraint(hop, hop->vreg_being_allocated);
	if (constraint && constraint->equals_to)
		mask = hop->inputregusage;

	for (i = startincl; i <= endincl; i++)
	{
		hop = current_hops.item[i];
		vreg = hop->vreg_being_allocated;

		if (i != 0)
			mask |= hop->inputregusage;
		if (i != (current_hops.count - 1))
			mask |= hop->outputregusage;
	}

	constraint = get_constraint(hop, vreg);
	if (constraint && (constraint->preserved || constraint->equals_to))
		mask |= hop->outputregusage;

	return mask;
}

#if 0
static uint32_t compute_constraints(
    struct basicblock* bb, int hopindex)
{
	uint32_t mask = 0;
	int count = vreg->used.count;
	struct hop* lasthop = vreg->used.item[count - 1];
	struct hop** hops = bb->hops.item;

	for (;;)
	{
		struct hop* hop = hops[hopindex];
		mask |= hop->outputregusage;
		if (hop == lasthop)
			break;

		hopindex++;
		hop = hops[hopindex];
		mask |= hop->inputregusage;
	}

	return mask;
}
#endif

static void assign_register_for_hop(
    struct hop* hop, struct vreg* vreg, struct hreg* hreg, bool write)
{
	struct constraint* constraint = get_constraint(hop, vreg);
	if (constraint && constraint->equals_to)
	{
		struct hreg* other
		    = pmap_findleft(&hop->assignments, constraint->equals_to);
		if (other)
		{
			tracef(
			    'R',
			    "R: hop %d couldn't allocate %%%d and %%%d to %s for equals-to "
			    "read/write because they are already assigned to %s\n",
			    hop->id, vreg->id, constraint->equals_to->id, hreg->id,
			    other->id);
		}
		else
		{
			tracef(
			    'R',
			    "R: hop %d allocated %%%d and %%%d to %s for equals-to "
			    "read/write\n",
			    hop->id, vreg->id, constraint->equals_to->id, hreg->id);
			pmap_put(&hop->assignments, vreg, hreg);
			pmap_put(&hop->assignments, constraint->equals_to, hreg);
			hop->inputregusage |= hreg->usage;
			hop->outputregusage |= hreg->usage;
		}
	}
	else if (constraint && constraint->preserved)
	{
		tracef(
		    'R', "R: hop %d allocated %%%d to %s for preserved read\n", hop->id,
		    vreg->id, hreg->id);
		assert(!pmap_findleft(&hop->assignments, vreg));
		hop->outputregusage |= hreg->usage;
		pmap_put(&hop->assignments, vreg, hreg);
		hop->inputregusage |= hreg->usage;
		hop->outputregusage |= hreg->usage;
	}
	else if (write)
	{
		tracef(
		    'R', "R: hop %d allocated %%%d to %s for write\n", hop->id,
		    vreg->id, hreg->id);
		assert(!pmap_findleft(&hop->assignments, vreg));
		pmap_put(&hop->assignments, vreg, hreg);
		hop->outputregusage |= hreg->usage;
	}
	else
	{
		tracef(
		    'R', "R: hop %d allocated %%%d to %s for read\n", hop->id, vreg->id,
		    hreg->id);
		assert(!pmap_findleft(&hop->assignments, vreg));
		pmap_put(&hop->assignments, vreg, hreg);
		hop->inputregusage |= hreg->usage;
	}
}

static void allocate_register_for_collected_hops(int startincl, int endincl)
{
	int i;
	uint32_t attrs = 0;
	struct hreg* hreg;
	struct constraint* constraint;
	regmask_t hint;
	regmask_t allocation_bitmap;

	for (i = startincl; i <= endincl; i++)
	{
		struct hop* hop = current_hops.item[i];
		constraint = get_constraint(hop, hop->vreg_being_allocated);
		if (constraint)
		{
			hreg = pmap_findleft(&hop->assignments, hop->vreg_being_allocated);
			if (hreg)
			{
				tracef(
				    'R', "R: hop %d already has %%%d allocated to %s\n",
				    hop->id, hop->vreg_being_allocated->id, hreg->id);
				hint |= hreg->usage;
			}

			if (constraint->equals_to)
			{
				hreg = pmap_findleft(&hop->assignments, constraint->equals_to);
				if (hreg)
				{
					tracef(
					    'R',
					    "R: hop %d has %%%d equals_to %%%d which is allocated "
					    "to %s\n",
					    hop->id, hop->vreg_being_allocated->id,
					    constraint->equals_to->id, hreg->id);
					hint |= hreg->usage;
				}
			}

			tracef(
			    'R', "R: hop %d touches %%%d with attribute requirement %s\n",
			    hop->id, hop->vreg_being_allocated->id,
			    render_regattrs(constraint->attrs));
			attrs = attrs | constraint->attrs;
		}
	}

	allocation_bitmap = compute_allocation_bitmap(startincl, endincl);
	hreg = allocate_register(allocation_bitmap, attrs);
	if (hreg)
	{
		for (i = startincl; i <= endincl; i++)
		{
			struct hop* hop = current_hops.item[i];

			assign_register_for_hop(
			    hop, hop->vreg_being_allocated, hreg, i == startincl);
		}
	}
	else
	{
		tracef(
		    'R', "R: no registers available for attribute requirement '%s'\n",
		    render_regattrs(attrs));
	}
}

static void local_register_allocation(void)
{
	int i, j;

	for (i = 0; i < dominance.preorder.count; i++)
	{
		struct basicblock* bb = dominance.preorder.item[i];
		tracef('R', "R: considering block %s\n", bb->name);

		/* Going through the hops in reverse means that we'll naturally
		 * allocate the shortest live regions first. */

		for (j = bb->hops.count - 1; j >= 0; j--)
		{
			struct vreg* vreg;
			struct hop* hop = bb->hops.item[j];
			if (hop->outs.count == 0)
				continue;

			assert(hop->outs.count == 1);
			vreg = hop->outs.item[0];
			assert(vreg->defined == hop);

			/* Collect all hops during the vreg's live range. */

			current_hops.count = 0;
			if (!collect_hops_for_vreg(bb, vreg))
				continue;

			tracef(
			    'R',
			    "R: considering candidate vreg %%%d with a %d-hop live range "
			    "from %d to %d\n",
			    vreg->id, current_hops.count, current_hops.item[0]->id,
			    current_hops.item[current_hops.count - 1]->id);
			allocate_register_for_collected_hops(0, current_hops.count - 1);
		}
	}
}

static void global_register_allocation(void)
{
}

void pass_register_allocator(void)
{
	populate_hregs();

	construct_interference_graph();

#if 0
	assign_hop_producer_consumers();
	local_register_allocation();
    global_register_allocation();
	//layout_stack_frame();
#endif
}

/* vim: set sw=4 ts=4 expandtab : */
