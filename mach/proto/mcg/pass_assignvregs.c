#include "mcg.h"

static struct basicblock* current_bb;

static int vregcount = 0;

static struct vreg* create_vreg(struct value* value)
{
    struct vreg* vreg = heap_alloc(&proc_heap, 1, sizeof(*vreg));
    vreg->id = vregcount++;
    vreg->value = value;
    return vreg;
}

static void create_and_map_vreg(struct hashtable* mapping, struct value* value)
{
    struct vreg* vreg = create_vreg(value);
    assert(!hashtable_get(mapping, value));
    hashtable_put(mapping, value, vreg);
}

static struct hop* create_move(struct hashtable* previous_mapping)
{
    struct hop* hop = new_hop(current_bb, NULL);
    hop->vregmapping = heap_alloc(&proc_heap, 1, sizeof(struct hashtable));;
    *hop->vregmapping = empty_hashtable_of_values;
    hop->is_move = true;

    struct hashtable_iterator hit = {};
    while (hashtable_next(previous_mapping, &hit))
    {
        struct value* value = hit.key;
        struct vreg* oldvreg = hit.value;
        struct vreg* newvreg = create_vreg(value);
        hashtable_put(hop->vregmapping, value, newvreg);
        pmap_add(&hop->copies, oldvreg, newvreg);
    }

    return hop;
}

static bool hop_reads_value(struct hop* hop, struct value* value)
{
    struct valueusage* usage = hop_get_value_usage(hop, value);
    return usage->input || usage->through;
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

    {
        struct hashtable_iterator hit = {};
        while (hashtable_next(hop->valueusage, &hit))
        {
            struct value* value = hit.key;
            struct valueusage* usage = hit.value;

            if (usage->input || usage->through)
            create_and_map_vreg(current_bb->inputmapping, value);
        }
    }

    previous_mapping = current_bb->inputmapping;
    for (i=0; i<current_bb->hops.count; i++)
    {
        /* Insert a parallel-move hop to copy all the vregs. */

        struct hop* move = create_move(previous_mapping);
        array_insert(&current_bb->hops, move, i);
        i++;
        previous_mapping = move->vregmapping;

        /* Copy the previous mapping to this hop, pruning out any unused values. */

        hop = current_bb->hops.item[i];
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

        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(hop->valueusage, &hit))
            {
                struct value* value = hit.key;
                struct valueusage* usage = hit.value;

                if (usage->output)
                    create_and_map_vreg(current_mapping, value);
            }
        }

        /* And move on to the next one. */

        previous_mapping = current_mapping;
    }

    /* Insert one final move at the end of the block. */

    hop = create_move(previous_mapping);
    array_append(&current_bb->hops, hop);
    current_bb->outputmapping = hop->vregmapping;
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

