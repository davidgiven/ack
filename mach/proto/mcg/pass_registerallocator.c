#include "mcg.h"
#include "bigraph.h"
#include "set.h"
#include <limits.h>

/* This is based around the elegant graph colouring algorithm here:
 *
 * http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.32.5924
 */

static const int DEGREE = 5;

static struct graph interference;
static struct graph affinity;

static struct vreg* actual(struct vreg* vreg)
{
    while (vreg->coalesced_with)
        vreg = vreg->coalesced_with;
    return vreg;
}

static void coalesce(struct vreg* v1, struct vreg* v2)
{
    tracef('R', "R: coalescing %%%d into %%%d\n", v1->id, v2->id);
    v1 = actual(v1);
    v2 = actual(v2);
    v1->coalesced_with = v2;
}

static void wire_together_bbs(void)
{
    int i, j;

    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];

        for (j=0; j<bb->imports.count; j++)
        {
            struct ir* src = bb->imports.item[j].left;
            struct ir* dest = bb->imports.item[j].right;
            struct vreg* srcvreg = hashtable_get(&src->bb->outputmapping, src);
            struct vreg* destvreg = hashtable_get(&bb->inputmapping, dest);
            if (srcvreg != destvreg)
                coalesce(srcvreg, destvreg);
        }
    }
}

static void generate_graph(void)
{
    int i, j, k;

    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];

        for (j=0; j<bb->hops.count; j++)
        {
            struct hop* hop = bb->hops.item[j];
            struct hashtable_iterator hit1 = {};
            while (hashtable_next(hop->valueusage, &hit1))
            {
                struct valueusage* usage1 = hit1.value;
                struct hashtable_iterator hit2 = {};
                while (hashtable_next(hop->valueusage, &hit2))
                {
                    struct valueusage* usage2 = hit2.value;
                    if (usage1 == usage2)
                        continue;

                    if (usage1->invreg && usage2->invreg)
                        graph_add_edge(&interference, actual(usage1->invreg), actual(usage2->invreg));
                    if (usage1->outvreg && usage2->outvreg)
                        graph_add_edge(&interference, actual(usage1->outvreg), actual(usage2->outvreg));

                    if (usage1->corrupted && usage1->invreg && usage2->outvreg)
                        graph_add_edge(&interference, actual(usage1->invreg), actual(usage2->outvreg));
                }

                if (hop->is_move && usage1->invreg && usage1->outvreg)
                    graph_add_edge(&affinity, actual(usage1->invreg), actual(usage1->outvreg));
            }
        }
    }
}

static void dump_vreg(struct vreg* vreg)
{
    fprintf(regalloc_dot_file, "[%%%d]", vreg->id);
    if (!vreg->needs_register)
        fprintf(regalloc_dot_file, "S");
}

static void dump_interference_graph(void)
{
    int i;

    if (!regalloc_dot_file)
        return;

    fprintf(regalloc_dot_file, "subgraph \"%s\" {\n", current_proc->name);

    {
        struct edge_iterator eit = {};
        while (graph_next_edge(&interference, &eit))
        {
            fprintf(regalloc_dot_file, "\t\"");
            dump_vreg(eit.left);
            fprintf(regalloc_dot_file, "\" -> \"");
            dump_vreg(eit.right);
            fprintf(regalloc_dot_file, "\";\n");
        }
    }

    {
        struct edge_iterator eit = {};
        while (graph_next_edge(&affinity, &eit))
        {
            fprintf(regalloc_dot_file, "\t\"");
            dump_vreg(eit.left);
            fprintf(regalloc_dot_file, "\" -> \"");
            dump_vreg(eit.right);
            fprintf(regalloc_dot_file, "\" [style=dotted];\n");
        }
    }

    #if 0
    {
        struct vertex_iterator vit = {};
        while (graph_next_vertex(&vertices, &vit))
        {
            struct anode* anode = sit.item;
            fprintf(regalloc_dot_file, "\t\"");
            dump_anode(anode);
            fprintf(regalloc_dot_file, "\" [color=green];\n");
        }
    }
    #endif

    fprintf(regalloc_dot_file, "}\n");
}

static bool attempt_to_coalesce(void)
{
    struct vreg* v1 = NULL;
    struct vreg* v2 = NULL;
    int degree = INT_MAX;

    /* Find the affinity edge with the lowest interference degree. */
    {
        struct edge_iterator eit = {};
        while (graph_next_edge(&affinity, &eit))
        {
            struct vreg* left = eit.left;
            struct vreg* right = eit.right;
            struct neighbour_iterator nit = {};
            static ARRAYOF(struct vreg) combined;

            combined.count = 0;
            while (graph_next_neighbour(&interference, left, &nit))
                array_appendu(&combined, nit.data);
            while (graph_next_neighbour(&interference, right, &nit))
                array_appendu(&combined, nit.data);

            if (combined.count < degree)
            {
                v1 = left;
                v2 = right;
                degree = combined.count;
            }
        }
    }

    if (!v1)
        return false;
    if (degree > DEGREE)
        return false;

    tracef('R', "R: coalescing affinity edge: %%%d->%%%d, degree %d\n",
        v1->id, v2->id, degree);
    coalesce(v1, v2);

    graph_merge_vertices(&affinity, v1, v2);
    if (graph_get_vertex_degree(&affinity, v1) == 0)
        graph_remove_vertex(&affinity, v1);

    graph_merge_vertices(&interference, v1, v2);

    return true;
}

static bool attempt_to_simplify(void)
{
    return false;
}

static void iterate(void)
{
    struct anode* spill;

    while (true)
    {
        tracef('R', "R: iterating; interference graph: %d, affinity graph: %d\n",
            interference.edges.table.size, affinity.edges.table.size);

        if (attempt_to_coalesce())
            continue;

        if (attempt_to_simplify())
            continue;

        // if (attempt_to_spill_or_simplify())
        //     continue;

        break;
    }
}

void pass_register_allocator(void)
{
    wire_together_bbs();
    generate_graph();

    //iterate();

    dump_interference_graph();

    graph_reset(&interference);
    graph_reset(&affinity);
}

/* vim: set sw=4 ts=4 expandtab : */
