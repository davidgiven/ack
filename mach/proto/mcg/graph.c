#include "mcg.h"

struct graph_data cfg;
struct dominance_data dominance;

static ARRAYOF(struct basicblock) pending;

static bool collect_outputs_cb(struct ir* ir, void* user)
{
    struct basicblock* caller = user;

    if (ir->opcode == IR_BLOCK)
    {
        array_appendu(&caller->nexts, ir->u.bvalue);
        array_appendu(&ir->u.bvalue->prevs, caller);
        pmap_add(&cfg.graph, caller, ir->u.bvalue);
    }
    return false;
}

static void update_block_pointers_from_ir(struct procedure* proc)
{
    int i, j;

    for (i=0; i<proc->blocks.count; i++)
    {
        struct basicblock* bb = proc->blocks.item[i];
        bb->prevs.count = bb->nexts.count = 0;
    }

    for (i=0; i<proc->blocks.count; i++)
    {
        struct basicblock* bb = proc->blocks.item[i];
        for (j=0; j<bb->irs.count; j++)
            ir_walk(bb->irs.item[j], collect_outputs_cb, bb);
    }

    for (i=0; i<proc->blocks.count; i++)
    {
        struct basicblock* bb = proc->blocks.item[i];

        for (j=0; j<bb->nexts.count; j++)
        {
            tracef('D', "D: cfg graph %s -> %s\n",
                bb->name,
                bb->nexts.item[j]->name);
        }
    }
}

static void recursively_walk_cfg_graph(struct basicblock* bb)
{
    int i;

    if (array_contains(&cfg.postorder, bb) || array_contains(&pending, bb))
        return;

    array_appendu(&cfg.preorder, bb);
    array_appendu(&pending, bb);

    for (i=0; i<bb->nexts.count; i++)
        recursively_walk_cfg_graph(bb->nexts.item[i]);

    array_remove(&pending, bb);
    bb->order = cfg.postorder.count;
    array_appendu(&cfg.postorder, bb);
}

static void walk_cfg_graph(void)
{
    int i;

    cfg.preorder.count = 0;
    cfg.postorder.count = 0;
    pending.count = 0;
    recursively_walk_cfg_graph(cfg.entry);

    for (i=0; i<cfg.preorder.count; i++)
    {
        tracef('G', "G: cfg preorder: %s\n",
            cfg.preorder.item[i]->name);
    }
}

static struct basicblock* intersect(struct basicblock* p1, struct basicblock* p2)
{
    while (p1 != p2)
    {
        while (p1->order < p2->order)
            p1 = pmap_findleft(&dominance.graph, p1);

        while (p2->order < p1->order)
            p2 = pmap_findleft(&dominance.graph, p2);
    }

    return p1;
}

static void calculate_dominance_graph(void)
{
    /* This is the algorithm described here:
     *
     * Cooper, Keith D., Timothy J. Harvey, and Ken Kennedy.
     * "A simple, fast dominance algorithm."
     * Software Practice & Experience 4.1-10 (2001): 1-8.
     *
     * https://www.cs.rice.edu/~keith/EMBED/dom.pdf
     */

    int i, j;
    bool changed;

    dominance.graph.count = 0;

    /* The entry block dominates itself. */

    pmap_put(&dominance.graph, cfg.entry, cfg.entry);

    do
    {
        changed = false;

        for (i = cfg.postorder.count-2; i >= 0; i--)
        {
            struct basicblock* b = cfg.postorder.item[i];
            struct basicblock* new_idom = NULL;
            for (j=0; j<b->prevs.count; j++)
            {
                struct basicblock* p = b->prevs.item[j];
                
                /* Skip unprocessed blocks. */
                if (!pmap_findleft(&dominance.graph, p))
                    continue;

                if (!new_idom)
                    new_idom = p;
                else if (pmap_findleft(&dominance.graph, p))
                    new_idom = intersect(p, new_idom);
            }

            if (pmap_findleft(&dominance.graph, b) != new_idom)
            {
                pmap_put(&dominance.graph, b, new_idom);
                changed = true;
            }
        }
    }
    while (changed);

    for (i=0; i<dominance.graph.count; i++)
    {
        tracef('G', "G: dominance graph: %s -> %s\n",
            dominance.graph.item[i].right->name,
            dominance.graph.item[i].left->name);
    }
}

static void recursively_walk_dominance_graph(struct basicblock* bb)
{
    int i;

    if (array_contains(&dominance.postorder, bb) || array_contains(&pending, bb))
        return;

    array_appendu(&dominance.preorder, bb);
    array_appendu(&pending, bb);

    for (i=0; i<dominance.graph.count; i++)
        if (dominance.graph.item[i].right == bb)
            recursively_walk_dominance_graph(dominance.graph.item[i].left);

    array_remove(&pending, bb);
    array_appendu(&dominance.postorder, bb);
}

static void walk_dominance_graph(void)
{
    int i;

    dominance.preorder.count = 0;
    dominance.postorder.count = 0;
    pending.count = 0;
    recursively_walk_dominance_graph(cfg.entry);

    for (i=0; i<dominance.preorder.count; i++)
    {
        tracef('G', "G: dominance preorder: %s\n",
            dominance.preorder.item[i]->name);
    }
}

void update_graph_data(struct procedure* proc)
{
    cfg.entry = proc->blocks.item[0];
    cfg.graph.count = 0;
    update_block_pointers_from_ir(proc);

    walk_cfg_graph();
    assert(cfg.postorder.count == proc->blocks.count);
    assert(cfg.preorder.count == proc->blocks.count);

    calculate_dominance_graph();
    
    walk_dominance_graph();
    assert(dominance.postorder.count == dominance.graph.count);
    assert(dominance.preorder.count == dominance.graph.count);
}

/* vim: set sw=4 ts=4 expandtab : */

