#include "mcg.h"
#include "bigraph.h"
#include "set.h"
#include "bitmap.h"
#include <limits.h>

/* This is based around the elegant graph colouring algorithm here:
 *
 * http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.32.5924
 */

static const int DEGREE = 20;

static struct graph interference;
static struct graph affinity;
static ARRAYOF(struct vreg) simplified;

static struct vreg* actual(struct vreg* vreg)
{
    while (vreg->coalesced_with)
        vreg = vreg->coalesced_with;
    return vreg;
}

static void coalesce(struct vreg* vmaster, struct vreg* vslave)
{
    vmaster = actual(vmaster);
    vslave = actual(vslave);
    vmaster->needs_register |= vslave->needs_register;
    vslave->coalesced_with = vmaster;
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

                    if (usage1->invreg)
                        graph_add_vertex(&interference, actual(usage1->invreg));
                    if (usage2->invreg)
                        graph_add_vertex(&interference, actual(usage2->invreg));
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
    struct vreg* vmaster = NULL;
    struct vreg* vslave = NULL;
    int degree = INT_MAX;

    /* Find the affinity edge with the lowest interference degree. */
    {
        struct edge_iterator eit = {};
        while (graph_next_edge(&affinity, &eit))
        {
            struct vreg* left = eit.left;
            struct vreg* right = eit.right;
            struct neighbour_iterator nit = {};
            int thisdegree = 0;

            while (graph_next_neighbour(&interference, left, &nit))
            {
                if (graph_get_vertex_degree(&interference, nit.data) >= DEGREE)
                    thisdegree++;
            }
            while (graph_next_neighbour(&interference, right, &nit))
            {
                if (graph_get_vertex_degree(&interference, nit.data) >= DEGREE)
                    thisdegree++;
            }

            if (thisdegree < degree)
            {
                vmaster = left;
                vslave = right;
                degree = thisdegree;
            }
        }
    }

    if (!vmaster || (degree > DEGREE))
        return false;

    tracef('R', "R: coalescing affinity edge: %%%d->%%%d, degree %d\n",
        vmaster->id, vslave->id, degree);
    coalesce(vmaster, vslave);

    graph_merge_vertices(&affinity, vmaster, vslave);
    if (graph_get_vertex_degree(&affinity, vslave) == 0)
        graph_remove_vertex(&affinity, vslave);

    graph_merge_vertices(&interference, vmaster, vslave);

    return true;
}

static bool attempt_to_simplify(void)
{
    struct vreg* vreg = NULL;
    int degree = INT_MAX;

    /* Find the vreg with the lowest degree that's not move-related (i.e.
     * has no affinity edges). */

    {
        struct vertex_iterator vit = {};
        while (graph_next_vertex(&interference, &vit))
        {
            struct vreg* v = vit.data;
            if (graph_get_vertex_degree(&affinity, v) == 0)
            {
                int d = graph_get_vertex_degree(&interference, v);
                if (d < degree)
                {
                    vreg = v;
                    degree = d;
                }
            }
        }
    }

    if (!vreg || (degree > DEGREE))
        return false;

    /* Push first the neighbours, and then the node itself annotated with the number
     * of neighbours, to the simplification stack. */

    tracef('R', "R: simplifying vreg %%%d with degree %d\n", vreg->id, degree);
    assert(vreg->neighbours == 0);
    {
        struct neighbour_iterator nit = {};
        while (graph_next_neighbour(&interference, vreg, &nit))
        {
            struct vreg* neighbour = nit.data;
            array_push(&simplified, neighbour);
            vreg->neighbours++;
        }
    }
    array_push(&simplified, vreg);

    /* ...and remove it from the graph. */

    graph_remove_vertex(&interference, vreg);
    graph_remove_vertex(&affinity, vreg);

    return true;
}

static bool attempt_to_freeze(void)
{
    struct vreg* vreg = NULL;
    int degree = INT_MAX;

    /* Find the affinity node with the lowest interference degree. */

    {
        struct vertex_iterator vit = {};
        while (graph_next_vertex(&affinity, &vit))
        {
            struct vreg* v = vit.data;
            int d = graph_get_vertex_degree(&interference, v);
            if (d < degree)
            {
                vreg = v;
                degree = d;
            }
        }
    }

    if (!vreg)
        return false;

    tracef('R', "R: freezing vreg %%%d with degree %d\n",
        vreg->id, degree);

    graph_remove_vertex(&affinity, vreg);

    return true;
}

static bool attempt_to_spill(void)
{
    struct vreg* vreg = NULL;
    int degree = INT_MIN;

    /* Find the spillable vreg with the *highest* degree. */

    {
        struct vertex_iterator vit = {};
        while (graph_next_vertex(&interference, &vit))
        {
            struct vreg* v = vit.data;
            if (!v->needs_register)
            {
                int d = graph_get_vertex_degree(&interference, v);
                if (d > degree)
                {
                    vreg = v;
                    degree = d;
                }
            }
        }
    }

    if (!vreg)
        return false;

    tracef('R', "R: spilling vreg %%%d with degree %d\n",
        vreg->id, degree);
    vreg->is_spilt = true;

    /* ...and remove it from the graph. */

    graph_remove_vertex(&interference, vreg);
    graph_remove_vertex(&affinity, vreg);

    return true;
}

static void iterate(void)
{
    struct anode* spill;

    while (interference.vertices.size > 0)
    {
        tracef('R', "R: iterating; interference graph: %d, affinity graph: %d\n",
            interference.edges.table.size, affinity.edges.table.size);

        if (attempt_to_simplify())
            continue;

        if (attempt_to_coalesce())
            continue;

        if (attempt_to_freeze())
            continue;

        // if (attempt_to_spill())
        //      continue;

        fatal("unable to allocate registers!");
    }
}

static void assign_hard_registers(void)
{
    while (simplified.count > 0)
    {
        struct vreg* vreg = array_pop(&simplified);
        int neighbours = vreg->neighbours;
        unsigned int bitmap = 0;

        while (neighbours > 0)
        {
            struct vreg* neighbour = actual(array_pop(&simplified));
            if (neighbour->hreg != -1)
                bitmap_set(&bitmap, 32, neighbour->hreg);
            neighbours--;
        }

        vreg->hreg = bitmap_find_unset_bit(&bitmap, 32);
    }
}

void pass_register_allocator(void)
{
    wire_together_bbs();
    generate_graph();

    dump_interference_graph();
    simplified.count = 0;
    iterate();
    assign_hard_registers();

    graph_reset(&interference);
    graph_reset(&affinity);
}

/* vim: set sw=4 ts=4 expandtab : */
