#include "mcg.h"
#include "mcgg.h"

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
	tracef('I', "I: 0x%p matched %s with cost %d vs. %d\n", p,
		burm_string[ruleno], cost, bestcost);
}

void burm_panic_cannot_match(struct ir* ir)
{
	fprintf(stderr, "could not find any patterns to match:\n");
	ir_print(0, ir);
	fprintf(stderr, "aborting!\n");
	exit(1);
}

static void queue_instructions(struct ir* ir, int goal)
{
	struct ir* children[10];
	int ruleno = burm_rule(ir->state_label, goal);
	const short* nts = burm_nts[ruleno];
	int i;

	burm_kids(ir, ruleno, children);
	for (i=0; nts[i]; i++)
		queue_instructions(children[i], nts[i]);

	printf("selected insn %d: %s\n", ruleno, burm_string[ruleno]);
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

		queue_instructions(ir, 1);
	}
}

void pass_instruction_selector(struct procedure* proc)
{
    int i;

    for (i=0; i<proc->blocks_count; i++)
    {
        struct basicblock* bb = proc->blocks[i];
        select_instructions(bb);
    }
	exit(1);
}

/* vim: set sw=4 ts=4 expandtab : */

