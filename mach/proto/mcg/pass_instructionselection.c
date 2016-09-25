#include "mcg.h"

static int vregcount;

#if 0
static void dumpCover(NODEPTR_TYPE p, int goalnt, int indent) {
#ifdef TRACE
	int eruleno = burm_rule(STATE_LABEL(p), goalnt);
	const short *nts = burm_nts[eruleno];
	NODEPTR_TYPE kids[10];
	int i;

	for (i = 0; i < indent; i++)
		fprintf(stderr, " ");
	fprintf(stderr, "%s\n", burm_string[eruleno]);
	burm_kids(p, eruleno, kids);
	for (i = 0; nts[i]; i++)
	{
		if (kids[i])
			dumpCover(kids[i], nts[i], indent + 1);
		else
			fprintf(stderr, "failed!\n");
	}
#endif
}
#endif

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

static const struct burm_emitter_data emitter_data;

static void emit_string(const char* data)
{
    tracef('I', "I: emit: %s\n", data);
}

static void emit_reg(struct ir* ir)
{
    const struct burm_instruction_data* insndata = &burm_instruction_data[ir->insn_no];
    if (insndata->is_fragment)
        insndata->emitter(ir, &emitter_data);
    else
        tracef('I', "I: emit reg %d\n", ir->vreg);
}

static void emit_value(struct ir* ir)
{
    tracef('I', "I: emit value\n");
}

static void emit_resultreg(void)
{
    tracef('I', "I: emit resultreg\n");
}

static void emit_eoi(void)
{
    tracef('I', "I: emit eoi\n");
}

static const struct burm_emitter_data emitter_data =
{
    &emit_string,
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
    int i;
    int resultreg = 0;

    ir->insn_no = insn_no;

    if (insndata->allocate)
    {
        resultreg = vregcount++;
        tracef('I', "I: new %s %d\n",
            burm_register_class_names[insndata->allocate], resultreg);
    }

    burm_kids(ir, insn_no, children);
    for (i=0; nts[i]; i++)
        walk_instructions(children[i], nts[i]);

    if (ir->vreg)
        tracef('I', "I: use %d\n", ir->vreg);
    if (resultreg)
        ir->vreg = resultreg;

    tracef('I', "I: $%d %s selected %s %d: %s\n",
        ir->id,
        ir->is_sequence ? "S" : " ",
        insndata->is_fragment ? "fragment" : "instruction",
        insn_no,
        insndata->name);
    ir->is_generated = true;

    if (!insndata->is_fragment && insndata->emitter)
        insndata->emitter(ir, &emitter_data);
}


static void select_instructions(struct basicblock* bb)
{
	int i;

	tracef('I', "I: BLOCK: %s\n", bb->name);

	for (i=0; i<bb->irs_count; i++)
	{
		int insnno;
		struct ir* ir = bb->irs[i];
		burm_label(ir);

		insnno = burm_rule(ir->state_label, 1);
		if (!insnno)
			burm_panic_cannot_match(ir);

		walk_instructions(ir, 1);
	}
}

void pass_instruction_selector(struct procedure* proc)
{
    int i;

    vregcount = 1;

    for (i=0; i<proc->blocks_count; i++)
    {
        struct basicblock* bb = proc->blocks[i];
        select_instructions(bb);
    }
}

/* vim: set sw=4 ts=4 expandtab : */

