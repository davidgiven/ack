#include "mcg.h"

struct procedure* current_proc;
struct heap proc_heap;

void* proc_alloc(size_t nmemb, size_t size)
{
    return heap_alloc(&proc_heap, nmemb, size);
}

void proc_free(void* ptr)
{
    /* do nothing; objects will leak until the heap is freed */
}

static void print_blocks(char k)
{
	int i;

	tracef(k, "%c: procedure %s\n", k, current_proc->name);
	for (i=0; i<current_proc->blocks.count; i++)
	{
		struct basicblock* bb = current_proc->blocks.item[i];
		int j;

        tracef(k, "%c:\n", k);
		tracef(k, "%c: %sBLOCK: %s\n", k,
            bb->is_fake ? "FAKE " : "",
            bb->name);

        if (bb->prevs.count > 0)
        {
            tracef(k, "%c: FROM:", k);
            for (j=0; j<bb->prevs.count; j++)
                tracef(k, " %s", bb->prevs.item[j]->name);
            tracef(k, "\n");
        }

        if (bb->nexts.count > 0)
        {
            tracef(k, "%c:   TO:", k);
            for (j=0; j<bb->nexts.count; j++)
                tracef(k, " %s", bb->nexts.item[j]->name);
            tracef(k, "\n");
        }

		for (j=0; j<bb->irs.count; j++)
			ir_print(k, bb->irs.item[j]);
	}
}

static void print_hops(char k)
{
    int i;

    tracef(k, "%c: procedure %s\n", k, current_proc->name);
    for (i=0; i<dominance.preorder.count; i++)
    {
		struct basicblock* bb = dominance.preorder.item[i];
		int j;

        tracef(k, "%c:\n", k);
		tracef(k, "%c: %sBLOCK: %s\n", k,
            bb->is_fake ? "FAKE " : "",
            bb->name);

        if (bb->prevs.count > 0)
        {
            tracef(k, "%c: FROM:", k);
            for (j=0; j<bb->prevs.count; j++)
                tracef(k, " %s", bb->prevs.item[j]->name);
            tracef(k, "\n");
        }

        if (bb->nexts.count > 0)
        {
            tracef(k, "%c:   TO:", k);
            for (j=0; j<bb->nexts.count; j++)
                tracef(k, " %s", bb->nexts.item[j]->name);
            tracef(k, "\n");
        }

        if (bb->imports.count > 0)
        {
            tracef(k, "%c:  INS:", k);
            for (j=0; j<bb->imports.count; j++)
                tracef(k, " $%d->$%d",
                    bb->imports.item[j].left->id, bb->imports.item[j].right->id);
            tracef(k, "\n");
        }

        if (bb->exports.count > 0)
        {
            tracef(k, "%c: OUTS:", k);
            for (j=0; j<bb->exports.count; j++)
                tracef(k, " $%d->$%d",
                    bb->exports.item[j].left->id, bb->exports.item[j].right->id);
            tracef(k, "\n");
        }

        if (bb->inputmapping.size > 0)
        {
            struct hashtable_iterator hit = {};
            tracef(k, "%c: INPUTMAPPING:", k);
            while (hashtable_next(&bb->inputmapping, &hit))
            {
                struct value* value = hit.key;
                struct vreg* vreg = hit.value;
                tracef(k, " $%d.%d->%%%d", value->ir->id, value->subid, vreg->id);
            }
            tracef(k, "\n");
        }

        if (bb->outputmapping.size > 0)
        {
            struct hashtable_iterator hit = {};
            tracef(k, "%c: OUTPUTMAPPING:", k);
            while (hashtable_next(&bb->inputmapping, &hit))
            {
                struct value* value = hit.key;
                struct vreg* vreg = hit.value;
                tracef(k, " $%d.%d->%%%d", value->ir->id, value->subid, vreg->id);
            }
            tracef(k, "\n");
        }

		for (j=0; j<bb->hops.count; j++)
			hop_print(k, bb->hops.item[j]);
    }
}

static void emit_procedure(struct procedure* proc)
{
    int i, j;

    fprintf(outputfile, "\n.sect .text\n");
    for (i=0; i<dominance.preorder.count; i++)
    {
        struct basicblock* bb = dominance.preorder.item[i];

        fprintf(outputfile, "%s:\n", platform_label(bb->name));
        for (j=0; j<bb->hops.count; j++)
        {
            struct hop* hop = bb->hops.item[j];
            fprintf(outputfile, "%s", hop_render(hop));
        }
    }
}

static void write_cfg_graph(const char* name)
{
    int i;

    fprintf(cfg_dot_file, "subgraph \"%s\" {\n", name);
    fprintf(cfg_dot_file, "\t\"%s\" [color=red];\n", cfg.entry->name);

    for (i=0; i<cfg.graph.count; i++)
    {
        fprintf(cfg_dot_file, "\t\"%s\" -> \"%s\";\n",
            cfg.graph.item[i].left->name,
            cfg.graph.item[i].right->name);
    }

    fprintf(cfg_dot_file, "}\n");
}

static void write_dominance_graph(const char* name)
{
    int i;

    fprintf(dominance_dot_file, "subgraph \"%s\" {\n", name);
    fprintf(dominance_dot_file, "\t\"%s\" [color=green];\n", cfg.entry->name);

    for (i=0; i<dominance.graph.count; i++)
    {
        fprintf(dominance_dot_file, "\t\"%s\" -> \"%s\";\n",
            dominance.graph.item[i].right->name,
            dominance.graph.item[i].left->name);
    }

    fprintf(dominance_dot_file, "}\n");
}

void procedure_compile(struct procedure* proc)
{
    current_proc = proc;
    tracef('P', "P: compiling %s\n", proc->name);

    pass_group_irs();
	print_blocks('a');

    /* Passes from here on must preserve IR grouping */

    pass_eliminate_trivial_blocks();
    pass_remove_dead_blocks();

    print_blocks('b');
    update_graph_data();
    print_blocks('c');
    pass_split_critical_edges();
    print_blocks('d');
    update_graph_data();

    if (cfg_dot_file)
        write_cfg_graph(proc->name);
    if (dominance_dot_file)
        write_dominance_graph(proc->name);

    /* Passes from here on can't alter the BB graph without also updating prevs
     * and nexts (and then calling update_graph_data()). */

    print_blocks('e');
    pass_wire_up_return_values();
    print_blocks('f');
    pass_convert_stack_ops();
    print_blocks('g');
    pass_convert_locals_to_ssa();
    print_blocks('h');
    pass_convert_inputs_to_phis();
    print_blocks('i');
    pass_convert_nonlocal_phis();
    print_blocks('j');
    pass_remove_dead_phis();
    print_blocks('k');
    pass_infer_types();
    print_blocks('l');
    pass_instruction_selector();
    print_hops('m');
    pass_live_value_analysis();
    print_hops('n');
    pass_assign_vregs();
    print_hops('o');
    pass_convert_copies_to_moves();
    print_hops('p');
    pass_collapse_adjacent_moves();
    print_hops('q');
    pass_prune_stray_moves();
    print_hops('r');
    pass_calculate_vreg_spillibility();
    print_hops('s');
    pass_register_allocator();
    print_hops('t');
#if 0
    pass_add_prologue_epilogue();
    print_hops('9');
#endif

    emit_procedure(proc);

    heap_free(&proc_heap);
}

/* vim: set sw=4 ts=4 expandtab : */

