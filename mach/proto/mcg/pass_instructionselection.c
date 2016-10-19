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
	ir_print('!', node->ir);
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
    {
        hop_add_vreg_insel(current_hop, vreg);
        array_appendu(&vreg->used, current_hop);
    }
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

static struct constraint* get_constraint(struct vreg* vreg)
{
    struct constraint* c = pmap_findleft(&current_hop->constraints, vreg);
    if (!c)
    {
        c = calloc(1, sizeof(*c));
        pmap_put(&current_hop->constraints, vreg, c);
    }
    return c;
}

static void constrain_input_reg(int child, uint32_t attr)
{
    struct vreg* vreg = find_vreg_of_child(child);
    struct constraint* c;

    if (vreg)
        array_appendu(&current_hop->ins, vreg);

    get_constraint(vreg)->attrs = attr;
}

static uint32_t find_type_from_constraint(uint32_t attr)
{
    /* Looks through the registers and finds a concrete register implementing
     * that attribute, and returns the type. We assume that all registers
     * implementing an attribute (which anyone is going to ask for, 'volatile'
     * doesn't count) will have the same type. TODO: mcgg should check for
     * this. */

    const struct burm_register_data* brd = burm_register_data;
    while (brd->name)
    {
        if (brd->attrs & attr)
            return brd->type;
        brd++;
    }

    fatal("unable to find a register matching attribute 0x%x", attr);
    return 0;
}

static void constrain_output_reg(uint32_t attr)
{
    struct vreg* vreg = current_hop->output;

    if (!vreg)
        current_hop->output = vreg = new_vreg();

    array_appendu(&current_hop->outs, vreg);
    vreg->defined = current_hop;
    vreg->type = find_type_from_constraint(attr);

    get_constraint(vreg)->attrs = attr;
}

static void constrain_output_reg_equal_to(int child)
{
    struct vreg* vreg = find_vreg_of_child(child);

    get_constraint(current_hop->output)->equals_to = vreg;
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
    &constrain_output_reg,
    &constrain_output_reg_equal_to,
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
            insn->hop = current_hop = new_hop(current_bb, insn->ir);
            current_hop->insndata = insn->insndata;
            emit(insn);

            if (!current_hop->output)
            {
                switch (node->label)
                {
                    case ir_to_esn(IR_REG, 1):
                    case ir_to_esn(IR_REG, 2):
                    case ir_to_esn(IR_REG, 4):
                    case ir_to_esn(IR_REG, 8):
                        current_hop->output = node->ir->result;
                        break;

                    case ir_to_esn(IR_NOP, 0):
                        current_hop->output = node->left->ir->result;
                        break;
                }
            }

            hop_print('I', current_hop);
            array_append(&current_bb->hops, current_hop);

            if ((goal != burm_stmt_NT) && !insn->ir->result)
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
            tracef('I', "I: $%d is phi:", current_ir->result->id);
            for (j=0; j<current_ir->u.phivalue.count; j++)
            {
                struct basicblock* parentbb = current_ir->u.phivalue.item[j].left;
                struct ir* parentir = current_ir->u.phivalue.item[j].right;
                struct phi* phi = calloc(1, sizeof(*phi));
                tracef('I', " %s=>$%d", parentbb->name, parentir->id);

                phi->prev = parentbb;
                phi->ir = parentir;
                pmap_add(&current_bb->phis, current_ir->result, phi);
            }
            tracef('I', "\n");
        }
        else
        {
            ir_print('I', current_ir);
            shadow = build_shadow_tree(current_ir, current_ir);
            burm_label(shadow);

            insnno = burm_rule(shadow->state_label, 1);
            if (!insnno)
                burm_panic_cannot_match(shadow);

            walk_instructions(shadow, burm_stmt_NT);
        }
	}
}

void pass_instruction_selector(void)
{
    int i;

    for (i=0; i<cfg.preorder.count; i++)
    {
        current_bb = cfg.preorder.item[i];
        select_instructions();
    }
}

/* vim: set sw=4 ts=4 expandtab : */

