#include "mcg.h"

static struct basicblock* current_bb;

static int vregcount = 0;
static struct hashtable mapping = HASHTABLE_OF_VALUES;

static struct vreg* create_vreg(struct value* value)
{
    struct vreg* vreg = heap_alloc(&proc_heap, 1, sizeof(*vreg));
    vreg->id = vregcount++;
    vreg->value = value;
    vreg->hreg = -1;
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
        usage->input = usage->output = true;
        hop_add_through_vreg(hop, oldvreg, newvreg);
    }

    return hop;
}

static bool hop_reads_value(struct hop* hop, struct value* value)
{
    struct valueusage* usage = hop_get_value_usage(hop, value);
    return usage->input || usage->through;
}

static void setup_regclass(struct vreg* vreg, struct value* value)
{
    const struct burm_regclass_data* regclass =
        (value->regclass == UNASSIGNED_REGCLASS)
        ? NULL
        : &burm_regclass_data[value->regclass];
    assert(!vreg->regclass || !regclass || (vreg->regclass == regclass));

	tracef('V', "V: %%%d has class %s; value $%d has class %s\n",
		vreg->id,
		vreg->regclass ? vreg->regclass->name : "?",
		value->ir->id,
		regclass ? regclass->name : "?");

    if (!vreg->regclass && regclass)
    {
        tracef('V', "V: assigning register class %s to %%%d from $%d\n",
            regclass->name, vreg->id, value->ir->id);
        memcpy(vreg->registers, regclass->bitmap, sizeof(burm_register_bitmap_t));
        vreg->regclass = regclass;
    }
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
        /* Remove completely unused vregs from the mapping. */

        hop = current_bb->hops.item[i];
        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(&mapping, &hit))
            {
                struct value* value = hit.key;
                struct valueusage* usage = hop_get_value_usage(hop, value);
                if (!usage->input && !usage->output && !usage->through)
                    hashtable_delete_current(&mapping, &hit);
            }
        }

        /* Insert a parallel-move hop to copy all the vregs. */

        {
            struct hop* move = clone_vregs();
            array_insert(&current_bb->hops, move, i);
            i++;
        }

        /* Copy the previous mapping to this hop. */

        hop = current_bb->hops.item[i];
        {
            struct hashtable_iterator hit = {};
            while (hashtable_next(&mapping, &hit))
            {
                struct value* value = hit.key;
                struct vreg* vreg = hit.value;

                hop_add_input_vreg(hop, vreg);
                setup_regclass(vreg, value);
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
                struct vreg* vreg = NULL;

                assert(!(usage->through && usage->output));
                if (usage->through)
                {
                    vreg = hop_find_input_vreg(hop, value);
                    hop_add_through_vreg(hop, vreg, vreg);
                }
                if (usage->output)
                {
                    vreg = create_and_map_vreg(value);
                    hop_add_output_vreg(hop, vreg);
                }

                if (vreg)
                    setup_regclass(vreg, value);
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

