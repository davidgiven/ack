#include "mcg.h"

#define MAX_CHILDREN 10

struct insn
{
    struct ir* ir;
    struct hop* hop;
    const struct burm_instruction_data* insndata;
    int num_children;
    struct insn* children[MAX_CHILDREN];
};

static struct basicblock* current_bb;
static struct hop* current_hop;
static struct ir* current_ir;
static struct insn* current_insn;

static void emit(struct insn* insn);

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

static void emit_return_reg(void)
{
    hop_add_vreg_insel(current_hop, current_hop->output);
}

static struct vreg* find_vreg_of_child(int child)
{
    struct insn* insn = current_insn->children[child];

    if (insn->hop)
        return insn->hop->output;
    else
        return insn->ir->result;
}

static void emit_reg(int child)
{
    struct vreg* vreg = find_vreg_of_child(child);

    if (vreg)
        hop_add_vreg_insel(current_hop, vreg);
}

static void emit_string(const char* data)
{
	hop_add_string_insel(current_hop, data);
}

static void emit_fragment(int child)
{
    emit(current_insn->children[child]);
}

static void emit_value(int child)
{
	hop_add_value_insel(current_hop, current_insn->children[child]->ir);
}

static void emit_eoi(void)
{
	hop_add_eoi_insel(current_hop);
}

static void constrain_input_reg(int child, int attr)
{
    struct vreg* vreg = find_vreg_of_child(child);

    if (vreg)
        array_appendu(&current_hop->ins, vreg);
}

static void constrain_output_reg(int attr)
{
}

static const struct burm_emitter_data emitter_data =
{
    &emit_string,
    &emit_fragment,
    &emit_return_reg,
    &emit_reg,
    &emit_value,
    &emit_eoi,
    &constrain_input_reg,
    &constrain_output_reg
};

static void emit(struct insn* insn)
{
    struct insn* old = current_insn;
    current_insn = insn;

    insn->insndata->emitter(&emitter_data);

    current_insn = old;
}

static struct insn* walk_instructions(struct burm_node* node, int goal)
{
    struct insn* insn = calloc(1, sizeof(*insn));
    int i;

    insn->ir = node->ir;
    insn->num_children = 0;

    if (goal)
    {
        int insn_no = burm_rule(node->state_label, goal);
        const short* nts = burm_nts[insn_no];
        struct burm_node* children[MAX_CHILDREN] = {0};

        insn->insndata = &burm_instruction_data[insn_no];

        burm_kids(node, insn_no, children);

        i = 0;
        for (;;)
        {
            if (!children[i])
                break;

            insn->children[i] = walk_instructions(children[i], nts[i]);
            insn->num_children++;
            i++;
        }

        tracef('I', "I: $%d goal %d %s selected %d: %s\n",
            node->ir->id,
            goal,
            insn->insndata->is_fragment ? "fragment" : "instruction",
            insn_no,
            insn->insndata->name);

        if (!insn->insndata->is_fragment)
        {
            struct vreg* vreg = NULL;

            switch (node->label)
            {
                case ir_to_esn(IR_REG, 0):
                    vreg = node->ir->result;
                    break;

                case ir_to_esn(IR_NOP, 0):
                    vreg = node->left->ir->result;
                    break;

                default:
                    /* FIXME: some instructions don't emit anything, so
                     * allocating a register for them is a waste of time. */
                    vreg = new_vreg();
            }

            insn->hop = current_hop = new_hop(0, insn->ir);
            insn->hop->output = vreg;
            if (vreg)
                array_appendu(&current_hop->outs, vreg);

            emit(insn);
            hop_print('I', current_hop);

            if (goal != 1)
                insn->ir->result = insn->hop->output;
        }
    }

    return insn;
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

        if (current_ir->opcode == IR_PHI)
        {
            int j;

            current_ir->result = new_vreg();
            array_append(&current_bb->liveins, current_ir->result);
            tracef('I', "I: %d is phi:", current_ir->result->id);
            for (j=0; j<current_ir->u.phivalue.count; j++)
                tracef('I', " $%d", current_ir->u.phivalue.item[j]->id);
            tracef('I', "\n");
        }
        else
        {
            shadow = build_shadow_tree(current_ir, current_ir);
            burm_label(shadow);

            insnno = burm_rule(shadow->state_label, 1);
            if (!insnno)
                burm_panic_cannot_match(shadow);

            ir_print('I', current_ir);
            walk_instructions(shadow, 1);
        }
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

