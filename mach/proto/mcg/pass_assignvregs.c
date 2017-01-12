#include "mcg.h"

static const struct hashtable empty_hashtable_of_values = HASHTABLE_OF_VALUES;

static struct basicblock* current_bb;

static int vregcount = 0;

static void create_and_map_vreg(struct hashtable* mapping, struct value* value)
{
    struct vreg* vreg = heap_alloc(&proc_heap, 1, sizeof(*vreg));
    vreg->id = vregcount++;
    vreg->value = value;

    assert(!hashtable_get(mapping, value));
    hashtable_put(mapping, value, vreg);
}

static bool hop_reads_value(struct hop* hop, struct value* value)
{
    int i;

    for (i=0; i<hop->inputs.count; i++)
        if (value_comparison_function(hop->inputs.item[i], value))
            return true;

    for (i=0; i<hop->throughs.count; i++)
        if (value_comparison_function(hop->throughs.item[i], value))
            return true;

    return false;
}

static void assign_vregs(void)
{
    int i, j;
    struct hashtable* previous_mapping;
    struct hashtable* current_mapping;
    struct hop* hop;

    /* Preload the mapping from the first hop. It might be a move, but that
     * does no harm. */

    hop = current_bb->hops.item[0];
    current_bb->inputmapping = heap_alloc(&proc_heap, 1, sizeof(struct hashtable));
    *current_bb->inputmapping = empty_hashtable_of_values;
    hashtable_empty(current_bb->inputmapping);
    for (i=0; i<hop->inputs.count; i++)
        create_and_map_vreg(current_bb->inputmapping, hop->inputs.item[i]);
    for (i=0; i<hop->throughs.count; i++)
        create_and_map_vreg(current_bb->inputmapping, hop->throughs.item[i]);

    previous_mapping = current_bb->inputmapping;
    for (i=0; i<current_bb->hops.count; i++)
    {
        hop = current_bb->hops.item[i];

        /* TODO: clone vregs here */

        /* Copy the previous mapping to this hop, pruning out any unused values. */

        current_mapping = hop->vregmapping = heap_alloc(&proc_heap, 1, sizeof(struct hashtable));;
        *current_mapping = empty_hashtable_of_values;
        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(previous_mapping, &hit))
            {
                struct value* value = hit.key;
                if (hop_reads_value(hop, value))
                    hashtable_put(current_mapping, value, hit.value);
            }
        }

        /* Create vregs for any new outputs. */

        for (j=0; j<hop->outputs.count; j++)
            create_and_map_vreg(current_mapping, hop->outputs.item[j]);

        /* And move on to the next one. */

        previous_mapping = current_mapping;
    }

    current_bb->outputmapping = previous_mapping;
}

void pass_assign_vregs(void)
{
    int i;

    vregcount = 0;

    for (i=0; i<dominance.preorder.count; i++)
    {
        current_bb = dominance.preorder.item[i];
        assign_vregs();
    }

    tracef('V', "V: finished vreg assignment with %d vregs\n", vregcount);
}

/* vim: set sw=4 ts=4 expandtab : */

