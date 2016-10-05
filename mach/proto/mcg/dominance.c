#include "mcg.h"

struct array postorder;
struct array preorder;
struct pmap dominators;

static void recursively_walk_blocks(struct basicblock* bb);

static void recursively_walk_graph(struct basicblock* bb)
{
    static ARRAYOF(struct basicblock) pending;
    int i;

    if (array_contains(&postorder, bb) || array_contains(&pending, bb))
        return;

    array_appendu(&preorder, bb);
    array_appendu(&pending, bb);

    i = 0;
    for (i=0; i<bb->nexts.count; i++)
        recursively_walk_graph(bb->nexts.item[i]);

    array_remove(&pending, bb);
    bb->order = postorder.count;
    array_appendu(&postorder, bb);
}

static void walk_graph_postorder(struct basicblock* entry)
{
    int i;

    preorder.count = 0;
    postorder.count = 0;
    recursively_walk_graph(entry);

    for (i=0; i<postorder.count; i++)
    {
        tracef('D', "D: postorder: %s\n",
            ((struct basicblock*)postorder.item[i])->name);
    }
}

static struct basicblock* intersect(struct basicblock* p1, struct basicblock* p2)
{
    while (p1 != p2)
    {
        while (p1->order < p2->order)
            p1 = pmap_get(&dominators, p1);

        while (p2->order < p1->order)
            p2 = pmap_get(&dominators, p2);
    }

    return p1;
}

void calculate_dominance_graph(struct procedure* proc)
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
    struct basicblock* entry = proc->blocks.item[0];

    entry = proc->blocks.item[0];
    walk_graph_postorder(entry);
    assert(postorder.count == proc->blocks.count);

    dominators.count = 0;

    /* The entry block dominates itself. */

    pmap_put(&dominators, entry, entry);

    do
    {
        changed = false;

        for (i = postorder.count-2; i >= 0; i--)
        {
            struct basicblock* b = postorder.item[i];
            struct basicblock* new_idom = NULL;
            for (j=0; j<b->prevs.count; j++)
            {
                struct basicblock* p = b->prevs.item[j];
                
                /* Skip unprocessed blocks. */
                if (!pmap_get(&dominators, p))
                    continue;

                if (!new_idom)
                    new_idom = p;
                else if (pmap_get(&dominators, p))
                    new_idom = intersect(p, new_idom);
            }

            if (pmap_get(&dominators, b) != new_idom)
            {
                pmap_put(&dominators, b, new_idom);
                changed = true;
            }
        }
    }
    while (changed);

    for (i=0; i<dominators.count; i++)
    {
        tracef('D', "D: domination: %s -> %s\n",
            ((struct basicblock*)dominators.item[i].left)->name,
            ((struct basicblock*)dominators.item[i].right)->name);
    }
}

/* vim: set sw=4 ts=4 expandtab : */

