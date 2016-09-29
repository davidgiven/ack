#include "mcg.h"

static struct basicblock* current_bb;
static struct hop* current_hop;

static const struct burm_emitter_data emitter_data;

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

static void emit_reg(struct ir* ir, int goal)
{
    hop_add_reg_insel(current_hop, ir, goal);
}

static void emit_string(const char* data)
{
	hop_add_string_insel(current_hop, data);
}

static void emit_fragment(struct ir* ir, int goal)
{
    int insn_no = burm_rule(ir->state_label, goal);
    const struct burm_instruction_data* insndata = &burm_instruction_data[insn_no];
    if (insndata->emitter)
        insndata->emitter(ir, &emitter_data);
}

static void emit_value(struct ir* ir)
{
	hop_add_value_insel(current_hop, ir);
}

static void emit_resultreg(void)
{
}

static void emit_eoi(void)
{
	hop_add_eoi_insel(current_hop);
}

static const struct burm_emitter_data emitter_data =
{
    &emit_string,
    &emit_fragment,
    &emit_reg,
    &emit_value,
    &emit_resultreg,
    &emit_eoi
};


static void walk_instructions(struct ir* ir, int goal)
{
    struct ir* children[10];
    int insn_no = burm_rule(ir->state_label, goal);
    const struct burm_instruction_data* insndata = &burm_instruction_data[insn_no];
    const short* nts = burm_nts[insn_no];
    struct hop* parent_hop = NULL;
    int i;
    
    if (!insndata->is_fragment)
    {
        parent_hop = current_hop;
        current_hop = new_hop(insn_no, ir);
    }

    burm_kids(ir, insn_no, children);
    for (i=0; nts[i]; i++)
        walk_instructions(children[i], nts[i]);

    ir->is_generated = true;
    ir->insn_no = insn_no;

    tracef('I', "I: $%d %s selected %s %d: %s\n",
        ir->id,
        ir->is_root ? "S" : " ",
        insndata->is_fragment ? "fragment" : "instruction",
        insn_no,
        insndata->name);

    if (!insndata->is_fragment)
    {
        if (insndata->emitter)
            insndata->emitter(ir, &emitter_data);

        hop_print('I', current_hop);
        array_append(&ir->hops, current_hop);
        current_hop = parent_hop;
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
		walk_instructions(ir, 1);
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

