#include "mcg.h"

static struct basicblock* current_bb;
static struct hop* current_hop;
static struct ir* current_ir;

static const struct burm_emitter_data emitter_data;

void burm_trace(struct burm_node* p, int ruleno, int cost, int bestcost) {
    const struct burm_instruction_data* insndata = &burm_instruction_data[ruleno];
	//tracef('I', "I: 0x%p matched %s with cost %d vs. %d\n", p,
	//	insndata->name, cost, bestcost);
}

void burm_panic_cannot_match(struct burm_node* node)
{
	fprintf(stderr, "could not find any patterns to match:\n");
	ir_print(0, node->ir);
	fprintf(stderr, "aborting!\n");
	exit(1);
}

static void emit_reg(struct burm_node* node, int goal)
{
    struct hop* hop = imap_get(&current_ir->hops, goal);

    hop_add_vreg_insel(current_hop, hop->output);
}

static void emit_string(const char* data)
{
	hop_add_string_insel(current_hop, data);
}

static void emit_fragment(struct burm_node* node, int goal)
{
    int insn_no = burm_rule(node->state_label, goal);
    const struct burm_instruction_data* insndata = &burm_instruction_data[insn_no];
    if (insndata->emitter)
        insndata->emitter(node, &emitter_data);
}

static void emit_value(struct burm_node* node)
{
	hop_add_value_insel(current_hop, node->ir);
}

static void emit_eoi(void)
{
	hop_add_eoi_insel(current_hop);
}

static void emit_constraint_equals(struct burm_node* node, int goal)
{
#if 0
    struct hop* hop;
    
    if (!goal)
        goal = ir->goal_no;
    hop = imap_get(&current_ir->hops, goal);

    current_hop->output = hop->output;
#endif
}

static const struct burm_emitter_data emitter_data =
{
    &emit_string,
    &emit_fragment,
    &emit_reg,
    &emit_value,
    &emit_eoi,
    &emit_constraint_equals
};


static void walk_instructions(struct burm_node* node, int goal)
{
    struct burm_node* children[10];
    int insn_no = burm_rule(node->state_label, goal);
    const struct burm_instruction_data* insndata = &burm_instruction_data[insn_no];
    const short* nts = burm_nts[insn_no];
    struct hop* parent_hop = NULL;
    struct ir* ir = node->ir;
    int i;
    
    if (!insndata->is_fragment)
    {
        parent_hop = current_hop;
        current_hop = new_hop(insn_no, ir);
        if (goal != 1)
        {
            current_hop->output = new_vreg();
            imap_add(&current_ir->hops, goal, current_hop);
        }
    }

    burm_kids(node, insn_no, children);
    for (i=0; nts[i]; i++)
        walk_instructions(children[i], nts[i]);

    tracef('I', "I: $%d goal %d selected %s %d: %s\n",
        ir->id,
        goal,
        insndata->is_fragment ? "fragment" : "instruction",
        insn_no,
        insndata->name);

    if (!insndata->is_fragment)
    {
        /* This may cause the vregs to be reassigned for this instruction (and
         * fragments contained within it). */

        insndata->emitter(node, &emitter_data);

        hop_print('I', current_hop);
        array_append(&ir->hops, current_hop);
        current_hop = parent_hop;
    }
}

static struct burm_node* build_shadow_tree(struct ir* root, struct ir* ir)
{
    struct burm_node* node = calloc(1, sizeof(*node));
    node->ir = ir;

    if (ir->root == root)
    {
        node->label = ir_to_esn(ir->opcode, ir->size);

        if (ir->left)
            node->left = build_shadow_tree(root, ir->left);

        if (ir->right)
            node->right = build_shadow_tree(root, ir->right);
    }
    else
        node->label = ir_to_esn(IR_REG, 0);

    return node;
}

static void select_instructions(void)
{
	int i;

	tracef('I', "I: BLOCK: %s\n", current_bb->name);

	for (i=0; i<current_bb->irs.count; i++)
	{
        struct burm_node* shadow;
		int insnno;

		current_ir = current_bb->irs.item[i];
        shadow = build_shadow_tree(current_ir, current_ir);
		burm_label(shadow);

		insnno = burm_rule(shadow->state_label, 1);
		if (!insnno)
			burm_panic_cannot_match(shadow);

        ir_print('I', current_ir);
		walk_instructions(shadow, 1);
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

