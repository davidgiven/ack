#include "mcg.h"

struct procedure* current_proc;
struct heap proc_heap;

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

        if (bb->liveins.count > 0)
        {
            tracef(k, "%c:  INS:", k);
            for (j=0; j<bb->liveins.count; j++)
                tracef(k, " %%%d", bb->liveins.item[j]->id);
            tracef(k, "\n");
        }

        if (bb->liveouts.count > 0)
        {
            tracef(k, "%c: OUTS:", k);
            for (j=0; j<bb->liveouts.count; j++)
                tracef(k, " %%%d", bb->liveouts.item[j]->id);
            tracef(k, "\n");
        }

        if (bb->phis.count > 0)
        {
            tracef(k, "%c: PHIS:", k);
            for (j=0; j<bb->phis.count; j++)
            {
                struct vreg* vreg = bb->phis.item[j].left;
                struct phi* phi = bb->phis.item[j].right;

                tracef(k, " %%%d(via %s)=>%%%d",
                    phi->ir->result->id,
                    phi->prev->name,
                    vreg->id);
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

    pass_group_irs();
	print_blocks('1');

    /* Passes from here on must preserve IR grouping */

    pass_eliminate_trivial_blocks();
    pass_remove_dead_blocks();

    print_blocks('2');
    update_graph_data();
    pass_split_critical_edges();
    update_graph_data();

    if (cfg_dot_file)
        write_cfg_graph(proc->name);
    if (dominance_dot_file)
        write_dominance_graph(proc->name);

    /* Passes from here on can't alter the BB graph without also updating prevs
     * and nexts (and then calling update_graph_data()). */

    print_blocks('3');
    pass_wire_up_return_values();
    pass_convert_stack_ops();
    pass_convert_locals_to_ssa();
    print_blocks('4');
    pass_convert_inputs_to_phis();
    pass_convert_nonlocal_phis();
    pass_remove_dead_phis();
    print_blocks('5');
    pass_infer_types();
    print_blocks('6');
    pass_instruction_selector();
    print_hops('7');
    pass_split_live_ranges();
    pass_determine_vreg_usage();
    print_hops('8');

    pass_live_vreg_analysis();
    pass_register_allocator();
    pass_add_prologue_epilogue();
    print_hops('9');

    emit_procedure(proc);

    heap_free(&proc_heap);
}

/* vim: set sw=4 ts=4 expandtab : */

