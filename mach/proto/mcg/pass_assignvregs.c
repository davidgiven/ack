#include "mcg.h"

static struct basicblock* current_bb;

static int vregcount = 0;
static struct hashtable mapping = HASHTABLE_OF_VALUES;

static struct vreg* create_vreg(struct value* value)
{
    struct vreg* vreg = heap_alloc(&proc_heap, 1, sizeof(*vreg));
    vreg->id = vregcount++;
    vreg->value = value;
    return vreg;
}

static struct vreg* create_and_map_vreg(struct value* value)
{
    struct vreg* vreg = create_vreg(value);
    assert(!hashtable_get(&mapping, value));
    hashtable_put(&mapping, value, vreg);
    return vreg;
}

static struct hop* clone_vregs(void)
{
    struct hop* hop = new_hop(current_bb, NULL);
    hop->is_move = true;

    struct hashtable_iterator hit = {};
    while (hashtable_next(&mapping, &hit))
    {
        struct value* value = hit.key;
        struct vreg* oldvreg = hit.value;
        struct vreg* newvreg = create_vreg(value);
        struct valueusage* usage = hop_get_value_usage(hop, value);

        hashtable_put(&mapping, value, newvreg);
        usage->input = true;
        usage->invreg = oldvreg;
        usage->output = true;
        usage->outvreg = newvreg;
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
    struct hop* hop;

    /* Preload the mapping from the first hop. It might be a move, but that
     * does no harm. */

    hop = current_bb->hops.item[0];
    assert(current_bb->inputmapping.size == 0);
    assert(current_bb->outputmapping.size == 0);
    assert(mapping.size == 0);
    current_bb->inputmapping = empty_hashtable_of_values;
    current_bb->outputmapping = empty_hashtable_of_values;

    {
        struct hashtable_iterator hit = {};
        while (hashtable_next(hop->valueusage, &hit))
        {
            struct value* value = hit.key;
            struct valueusage* usage = hit.value;

            if (usage->input || usage->through)
            {
                struct vreg* vreg = create_and_map_vreg(value);
                hashtable_put(&current_bb->inputmapping, value, vreg);
            }
        }
    }

    for (i=0; i<current_bb->hops.count; i++)
    {
        /* Insert a parallel-move hop to copy all the vregs. */

        struct hop* move = clone_vregs();
        array_insert(&current_bb->hops, move, i);
        i++;

        /* Copy the previous mapping to this hop. */

        hop = current_bb->hops.item[i];
        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(&mapping, &hit))
            {
                struct value* value = hit.key;
                struct vreg* vreg = hit.value;
                hop_get_value_usage(hop, value)->invreg = vreg;
            }
        }

        /* Prune any unused values from the mapping. */

        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(hop->valueusage, &hit))
            {
                struct value* value = hit.key;
                struct valueusage* usage = hit.value;
                if (!usage->through)
                    hashtable_remove(&mapping, value);
            }
        }

        /* Handle new output vregs, and propagated through vregs. */

        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(hop->valueusage, &hit))
            {
                struct value* value = hit.key;
                struct valueusage* usage = hit.value;

                assert(!(usage->through && usage->output));
                if (usage->through)
                    usage->outvreg = usage->invreg;
                if (usage->output)
                    usage->outvreg = create_and_map_vreg(value);
            }
        }
    }

    /* The last hop in the block is always an exit: either a branch or a ret
     * or something like that. So the output mapping we want is the state of
     * the registers on exit from this instruction. */

    hashtable_copy_all(&mapping, &current_bb->outputmapping);
}

void pass_assign_vregs(void)
{
    int i;

    for (i=0; i<dominance.preorder.count; i++)
    {
        current_bb = dominance.preorder.item[i];
        assign_vregs();
        hashtable_empty(&mapping);
    }

    hashtable_reset(&mapping);
    tracef('V', "V: finished vreg assignment with %d vregs\n", vregcount);
}

/* vim: set sw=4 ts=4 expandtab : */

