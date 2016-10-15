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
	
    struct ir* jump = 
		new_ir1(
			IR_JUMP, 0,
			new_bbir(sink)
		);

    jump->root = jump->left->root = jump;
	array_append(&bb->irs, jump);

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

static bool consider_edges_leading_from(struct basicblock* bb)
{
    bool changed = false;

	if (bb->nexts.count > 1)
	{
		int i;

		for (i=0; i<bb->nexts.count; i++)
		{
			struct basicblock* next = bb->nexts.item[i];
			if (next->prevs.count > 1)
            {
				split_edge(bb, next);
                changed = true;
            }
		}
	}

    return changed;
}

void pass_split_critical_edges(void)
{
	int i;
    bool changed;

    do
    {
        changed = false;

        for (i=0; i<current_proc->blocks.count; i++)
            changed |= consider_edges_leading_from(current_proc->blocks.item[i]);

    }
    while (changed);
}

/* vim: set sw=4 ts=4 expandtab : */

