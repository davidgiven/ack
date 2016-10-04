#include "mcg.h"

/* Insert empty nodes at certain places in the basic block graph so that when
 * we convert out of SSA form, we have somewhere to insert copies. This is
 * necessary for correctness in certain circumstances. The best explanation of
 * why I've found is here, starting at the bottom of page 23.
 *
 * Briggs, Preston, et al.
 * "Practical improvements to the construction and destruction of static single assignment form."
 * Software-Practice and experience 28.8 (1998): 859-882.
 *
 * http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.25.749
 */

static struct procedure* current_proc;

struct rewrite_params
{
	struct basicblock* find;
	struct basicblock* replace;
};

static bool find_replace_cb(struct ir* ir, void* user)
{
	struct rewrite_params* rwp = user;

	if ((ir->opcode == IR_BLOCK) && (ir->u.bvalue == rwp->find))
		ir->u.bvalue = rwp->replace;
	
	return false;
}

static void split_edge(struct basicblock* source, struct basicblock* sink)
{
	int i;
	struct rewrite_params rwp;
	struct basicblock* bb = bb_get(NULL);
	
	array_append(&bb->irs,
		new_ir1(
			IR_JUMP, 0,
			new_bbir(sink)
		)
	);

	rwp.find = sink;
	rwp.replace = bb;

	for (i=0; i<source->irs.count; i++)
		ir_walk(source->irs.item[i], find_replace_cb, &rwp);

	array_remove(&source->nexts, sink);
	array_append(&source->nexts, bb);

	array_append(&bb->prevs, source);
	array_append(&bb->nexts, sink);

	array_remove(&sink->prevs, source);
	array_append(&sink->prevs, bb);

	array_append(&current_proc->blocks, bb);
}

static void consider_edges_leading_to(struct basicblock* bb)
{
	if (bb->prevs.count > 1)
	{
		int i;

		for (i=0; i<bb->prevs.count; i++)
		{
			struct basicblock* prev = bb->prevs.item[i];
			if (prev->nexts.count > 1)
				split_edge(prev, bb);
		}
	}
}

void pass_split_critical_edges(struct procedure* proc)
{
	int i;

	current_proc = proc;

    for (i=0; i<proc->blocks.count; i++)
		consider_edges_leading_to(proc->blocks.item[i]);
}

/* vim: set sw=4 ts=4 expandtab : */


