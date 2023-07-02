#include "mcg.h"

static ARRAYOF(struct hreg) hregs;
static struct bitset interference_graph;
static ARRAYOF(struct vreg) all_vregs;
static ARRAYOF(struct hop) current_hops;
static ARRAYOF(struct vreg) register_stack;

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
		    'R', "R: found hreg %s, attrs %s, aliases %08x\n", hreg->id,
		    render_regclasses(hreg->regclasses), hreg->usage);
	}
}

static void coalesce_registers(struct vreg* v1, struct vreg* v2)
{
	if (v1->coalesced_with)
		v1 = v1->coalesced_with;
	if (v2->coalesced_with)
		v2 = v2->coalesced_with;
	if (v1 == v2)
		return;

	tracef('R', "R: coalescing %%%d and %%%d\n", v1->id, v2->id);
	while (v2)
	{
		struct vreg* nextv = v2->next_coalesced_register;
		v2->coalesced_with = v1;
		v2->next_coalesced_register = v1->next_coalesced_register;
		v1->next_coalesced_register = v2;
		v2 = nextv;
	}
	v1->coalesced_with = v1;
}

static void coalesce_equal_registers(void)
{
	int i, j, k, l;

	for (i = 0; i < dominance.preorder.count; i++)
	{
		struct basicblock* bb = dominance.preorder.item[i];

		/* Coalesce the block's phis with their parents. */

		for (j = 0; j < bb->phis.count; j++)
		{
			struct vreg* vreg = bb->phis.item[j].left;
			struct phi* phi = bb->phis.item[j].right;
			tracef(
			    'R', "R: coalescing phi registers %%%d and %%%d\n", vreg->id,
			    phi->ir->result->id);
			vreg->in_transit = true;
			phi->ir->result->in_transit = true;
			coalesce_registers(vreg, phi->ir->result);
		}

		/* Handle any equals_to constraints. */

		for (j = 0; j < bb->hops.count; j++)
		{
			struct hop* hop = bb->hops.item[j];

			for (k = 0; k < hop->constraints.count; k++)
			{
				struct vreg* vreg = hop->constraints.item[k].left;
				struct constraint* constraint = hop->constraints.item[k].right;
				if (constraint->equals_to)
					coalesce_registers(vreg, constraint->equals_to);
			}
		}
	}
}

static void mark_vreg_interference(struct vreg* v1, struct vreg* v2)
{
	if (v1->coalesced_with)
		v1 = v1->coalesced_with;
	if (v2->coalesced_with)
		v2 = v2->coalesced_with;
	if (v1->is_spilt || v2->is_spilt)
		return;

	assert(v1->id != 0);
	assert(v2->id != 0);
	bitset_set(&interference_graph, v1->id * vreg_count + v2->id, true);
	bitset_set(&interference_graph, v2->id * vreg_count + v1->id, true);
}

static void mark_vreg_list_interference(
    struct vreg** v1, size_t v1n, struct vreg** v2, size_t v2n)
{
	size_t i, j;

	for (i = 0; i < v1n; i++)
	{
		struct vreg* r1 = v1[i];
		for (j = 0; j < v2n; j++)
		{
			struct vreg* r2 = v2[j];
			mark_vreg_interference(r1, r2);
		}
	}
}

static struct vreg* root_vreg(struct vreg* vreg)
{
	if (vreg->coalesced_with)
		return vreg->coalesced_with;
	return vreg;
}

static void construct_interference_graph(void)
{
	int i, j, k, l;

	tracef(
	    'R', "R: creating interference matrix of %dx%d vregs\n", vreg_count,
	    vreg_count);
	bitset_resize(&interference_graph, vreg_count * vreg_count);
	bitset_clear(&interference_graph);
	array_resize(&all_vregs, vreg_count);
	array_clear(&all_vregs);

	for (i = 0; i < dominance.preorder.count; i++)
	{
		struct basicblock* bb = dominance.preorder.item[i];

		/* Process each hop in the basic block. */

		for (j = 0; j < bb->hops.count; j++)
		{
			struct hop* hop = bb->hops.item[j];
			struct vreg** ins = hop->ins.item;
			int nins = hop->ins.count;
			struct vreg** outs = hop->outs.item;
			int nouts = hop->outs.count;
			struct vreg** throughs = hop->throughs.item;
			int nthroughs = hop->throughs.count;

			/* Record all used vregs. */

			for (k = 0; k < nins; k++)
				all_vregs.item[ins[k]->id] = root_vreg(ins[k]);
			for (k = 0; k < nthroughs; k++)
				all_vregs.item[throughs[k]->id] = root_vreg(throughs[k]);
			for (k = 0; k < nouts; k++)
				all_vregs.item[outs[k]->id] = root_vreg(outs[k]);

			/* Each list of registers interferences with itself. */

			mark_vreg_list_interference(ins, nins, ins, nins);
			mark_vreg_list_interference(
			    throughs, nthroughs, throughs, nthroughs);
			mark_vreg_list_interference(outs, nouts, outs, nouts);

			/* Ins and outs both interfere with throughs. */

			mark_vreg_list_interference(ins, nins, throughs, nthroughs);
			mark_vreg_list_interference(outs, nouts, throughs, nthroughs);

			/* Any preserved input registers interfere with all output
			 * registers. */

			for (k = 0; k < nins; k++)
			{
				struct vreg* v = ins[k];
				struct constraint* constraint = get_constraint(hop, v);
				if (constraint && constraint->preserved)
					mark_vreg_list_interference(&v, 1, outs, nouts);
			}
		}
	}
}

static void account_register(struct hop* hop, struct vreg* vreg)
{
	if (vreg->coalesced_with)
		vreg = vreg->coalesced_with;
	if (vreg->is_spilt)
		return;

	if (hop->id < vreg->min_hop)
		vreg->min_hop = hop->id;
	if (hop->id > vreg->max_hop)
		vreg->max_hop = hop->id;
	vreg->use_count++;
}

static void spill_conflicting_register(void)
{
	int i, j, k;
	struct vreg* bestvreg;
	float bestscore;

	for (i = 0; i < all_vregs.count; i++)
	{
		struct vreg* vreg = all_vregs.item[i];
		if (vreg && (!vreg->coalesced_with || (vreg->coalesced_with == vreg)))
		{
			vreg->min_hop = INT_MAX;
			vreg->max_hop = 0;
			vreg->use_count = 0;

			vreg->interference_count = 0;
			for (j = 0; j < all_vregs.count; j++)
				vreg->interference_count
				    += bitset_get(&interference_graph, i * vreg_count + j);
		}
	}

	for (i = 0; i < dominance.preorder.count; i++)
	{
		struct basicblock* bb = dominance.preorder.item[i];

		for (j = 0; j < bb->hops.count; j++)
		{
			struct hop* hop = bb->hops.item[j];

			/* Record all used vregs. */

			for (k = 0; k < hop->ins.count; k++)
				account_register(hop, hop->ins.item[k]);
			for (k = 0; k < hop->throughs.count; k++)
				account_register(hop, hop->throughs.item[k]);
			for (k = 0; k < hop->outs.count; k++)
				account_register(hop, hop->outs.item[k]);
		}
	}

	bestvreg = NULL;
	bestscore = 0.0;
	for (i = 0; i < all_vregs.count; i++)
	{
		struct vreg* vreg = all_vregs.item[i];
		if (vreg && vreg->in_transit && !vreg->is_spilt)
		{
			float score
			    = (float)(vreg->max_hop - vreg->min_hop) / vreg->use_count;
			assert(!vreg->coalesced_with || (vreg->coalesced_with == vreg));
			tracef(
			    'R',
			    "R: %%%d has minhop %d maxhop %d usecount %d interference %d "
			    "score %f\n",
			    vreg->id, vreg->min_hop, vreg->max_hop, vreg->use_count,
			    vreg->interference_count, score);
			if (score > bestscore)
			{
				bestvreg = vreg;
				bestscore = score;
			}
		}
		else if (vreg)
		{
			tracef(
			    'R',
			    "R: ignoring %%%d for spill with transit %d and spill %d\n",
			    vreg->id, vreg->in_transit, vreg->is_spilt);
		}
	}

	if (!bestvreg)
		fatal("cannot find a register to spill");

	tracef('R', "R: selected %%%d for spilling\n", bestvreg->id);
	bestvreg->is_spilt = true;
}

static bool colour_graph(void)
{
	int i, j, k;
	int stacked;
	bool spilled;

	register_stack.count = 0;
	do
	{
		stacked = 0;
		spilled = false;
		for (i = 0; i < vreg_count; i++)
		{
			struct vreg* vreg = all_vregs.item[i];
			if (vreg && !vreg->is_spilt)
			{
				size_t bitoffset = i * vreg_count;
				int qvalue = 0;
				int pvalue = burm_register_class_p_table[vreg->regclass];
				int count;

				assert(!vreg->coalesced_with || (vreg->coalesced_with == vreg));
				for (j = 0; j < vreg_count; j++)
				{
					if (bitset_get(&interference_graph, bitoffset + j))
					{
						struct vreg* other = all_vregs.item[j];
						if (other && !other->is_spilt)
							qvalue += burm_register_class_q_table
							    [(vreg->regclass * BURM_REGISTER_CLASS_COUNT)
							     + other->regclass];
					}
				}

				if (qvalue != 0)
				{
					tracef(
					    'R',
					    "R: vreg %s of class %s has q=%d and p="
					    "%d "
					    "and "
					    "intersects with:",
					    render_vreg(vreg), render_regclass(vreg->regclass),
					    qvalue, pvalue);
					count = 0;
					for (k = 0; k < vreg_count; k++)
					{
						if ((k != vreg->id)
						    && bitset_get(
						        &interference_graph, vreg->id * vreg_count + k))
						{
							struct vreg* other = all_vregs.item[k];
							tracef('R', " %s", render_vreg(other));
							count++;
						}
					}
					tracef('R', " (%d registers) ", count);

					/* Perform the colourability test. */

					if (qvalue > pvalue)
					{
						tracef('R', "\n");
						spilled = true;
					}
					else
					{
						/* Stack this register and remove it from the
						 * interference matrix.
						 */

						tracef('R', "STACKING\n");
						stacked++;
						array_push(&register_stack, vreg);
						for (k = 0; k < vreg_count; k++)
						{
							bitset_set(
							    &interference_graph, vreg->id * vreg_count + k,
							    false);
							bitset_set(
							    &interference_graph, k * vreg_count + vreg->id,
							    false);
						}
					}
				}
			}
		}
		tracef(
		    'R', "R: %s scan with %d stacked\n",
		    spilled ? "failed" : "successful", stacked);
	} while (stacked);

	/* If we reach the end of the list, we've either stacked everything (in
	 * which case colouring has succeeded) or else some registers are
	 * uncolourable. */

	if (spilled)
		tracef('R', "R: colouring failed\n");
	else
		tracef('R', "R: colouring successful\n");

	return !spilled;
}

#if 0
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
#endif

static void assign_registers(void)
{
	int i;

	tracef('R', "R: assigning %d registers\n", register_stack.count);
	while (register_stack.count != 0)
	{
		struct vreg* vreg = array_pop(&register_stack);
		regbits_t usage = 0;

		assert(!vreg->is_spilt);
		for (i = 0; i < vreg_count; i++)
		{
			if (bitset_get(&interference_graph, vreg->id * vreg_count + i))
			{
				struct vreg* other = all_vregs.item[i];
				if (other->hreg)
					usage |= other->hreg->usage;
			}
		}

		for (i = 0; i < hregs.count; i++)
		{
			struct hreg* hreg = hregs.item[i];
			if (!(hreg->usage & usage)
			    && (hreg->regclasses & (1 << vreg->regclass)))
			{
				tracef(
				    'R', "R: assigning %s to %s\n", hreg->id,
				    render_vreg(vreg));
				vreg->hreg = hreg;
				break;
			}
		}
		if (!vreg->hreg)
			fatal(
			    "failed to get a register for %s with class %s; usage "
			    "bitmap "
			    "%x",
			    render_vreg(vreg), render_regclass(vreg->regclass), usage);
	}
}

static bool iterate(void)
{
	construct_interference_graph();
	if (!colour_graph())
	{
		spill_conflicting_register();
		return false;
	}

	construct_interference_graph();
	assign_registers();
	return true;
}

void pass_register_allocator(void)
{
	int count = 1;
	populate_hregs();

	coalesce_equal_registers();
	do
	{
		tracef('R', "R: beginning graph colouring iteration %d\n", count);
		count++;
	} while (!iterate());
}

/* vim: set sw=4 ts=4 expandtab : */
