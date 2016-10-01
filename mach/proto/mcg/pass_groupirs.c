#include "mcg.h"

static ARRAYOF(struct ir) allirs;
static ARRAYOF(struct ir) rootirs;

static void addall(struct ir* ir)
{
    if (array_appendu(&allirs, ir))
        return;

    if (ir->left)
        addall(ir->left);
    if (ir->right)
        addall(ir->right);
}

static void collect_irs(struct procedure* proc)
{
    int i;
    
    allirs.count = rootirs.count = 0;
	for (i=0; i<proc->blocks.count; i++)
    {
        struct basicblock* bb = proc->blocks.item[i];
        int j;

        for (j=0; j<bb->irs.count; j++)
		{
			struct ir* ir = bb->irs.item[j];
            addall(ir);
			array_appendu(&rootirs, ir);
		}
    }
}

static void clear_roots(void)
{
	int i;

	for (i=0; i<allirs.count; i++)
	{
		struct ir* ir = allirs.item[i];
		ir->root = NULL;
	}
}

static void find_roots(void)
{
	int i;

	for (i=0; i<rootirs.count; i++)
	{
		struct ir* ir = rootirs.item[i];
		assert(!ir->root);
		ir->root = ir;
	}
}

static void recursively_mark_root(struct ir* node, struct ir* root)
{
	if (node != root)
	{
		if (node->root)
			return;
		node->root = root;
	}

	if (node->left)
		recursively_mark_root(node->left, root);
	if (node->right)
		recursively_mark_root(node->right, root);
}

static void find_non_roots(void)
{
	int i;

	for (i=0; i<rootirs.count; i++)
	{
		struct ir* ir = rootirs.item[i];
		recursively_mark_root(ir, ir);
	}
}

void pass_group_irs(struct procedure* proc)
{
	collect_irs(proc);
	clear_roots();
	find_roots();
	find_non_roots();
}

/* vim: set sw=4 ts=4 expandtab : */


