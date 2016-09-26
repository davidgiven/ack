#include "mcg.h"

static struct basicblock* current_bb;

void burm_trace(struct ir* p, int ruleno, int cost, int bestcost) {
    const struct burm_instruction_data* insndata = &burm_instruction_data[ruleno];
	//tracef('I', "I: 0x%p matched %s with cost %d vs. %d\n", p,
	//	insndata->name, cost, bestcost);
}

void burm_panic_cannot_match(struct ir* ir)
{
	fprintf(stderr, "could not find any patterns to match:\n");
	ir_print(0, ir);
	fprintf(stderr, "aborting!\n");
	exit(1);
}

static void walk_instructions(struct hop* hop, struct ir* ir, int goal)
{
    struct ir* children[10];
    int insn_no = burm_rule(ir->state_label, goal);
    const struct burm_instruction_data* insndata = &burm_instruction_data[insn_no];
    const short* nts = burm_nts[insn_no];
    int i;
    
    ir->insn_no = insn_no;
    if (!insndata->is_fragment)
        hop = ir->hop = new_hop(insn_no, ir);;

    if (insndata->allocate)
    {
        hop->resultvreg = new_vreg();
        array_append(&hop->newvregs, hop->resultvreg);
        array_append(&hop->outvregs, hop->resultvreg);
    }

    burm_kids(ir, insn_no, children);
    for (i=0; nts[i]; i++)
        walk_instructions(hop, children[i], nts[i]);

    ir->is_generated = true;

    tracef('I', "I: $%d %s selected %s %d: %s\n",
        ir->id,
        ir->is_sequence ? "S" : " ",
        insndata->is_fragment ? "fragment" : "instruction",
        insn_no,
        insndata->name);

    if (!insndata->is_fragment)
    {
        array_append(&current_bb->hops, hop);
        hop_print('I', hop);
    }
}


static void select_instructions(void)
{
	int i;

	tracef('I', "I: BLOCK: %s\n", current_bb->name);

	for (i=0; i<current_bb->irs.count; i++)
	{
		int insnno;
		struct ir* ir = current_bb->irs.item[i];
		burm_label(ir);

		insnno = burm_rule(ir->state_label, 1);
		if (!insnno)
			burm_panic_cannot_match(ir);

        ir_print('I', ir);
		walk_instructions(NULL, ir, 1);
	}
}

void pass_instruction_selector(struct procedure* proc)
{
    int i;

    for (i=0; i<proc->blocks.count; i++)
    {
        current_bb = proc->blocks.item[i];
        select_instructions();
    }
}

/* vim: set sw=4 ts=4 expandtab : */

