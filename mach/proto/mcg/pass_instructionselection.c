#include "mcg.h"

#define MAX_CHILDREN 10

struct insn
{
    /* struct insn must be castable to a struct value */
    struct value value;

    struct ir* ir;
    struct hop* hop;
    const struct burm_instruction_data* insndata;
    int num_children;
    struct insn* children[MAX_CHILDREN];
};

static struct basicblock* current_bb;
static struct ir* root_ir;
static int hop_id;
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

static void emit_return_reg(int index)
{
    hop_add_vreg_insel(current_hop, &current_insn->value, index);
}

static struct value* find_value_of_child(int child)
{
    struct insn* insn = current_insn->children[child];

    if (insn->hop)
        return insn->hop->value;
    else
        return &insn->ir->value;
}

static void emit_reg(int child, int index)
{
    struct value* value = find_value_of_child(child);

    assert(value);
    hop_add_vreg_insel(current_hop, value, index);
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

static void constrain_input_reg(int child, uint32_t attr)
{
    struct value* value = find_value_of_child(child);
    struct constraint* c;

    array_appendu(&current_hop->inputs, value);
    value->attrs = attr;
}

static void constrain_input_reg_corrupted(int child)
{
    struct value* value = find_value_of_child(child);
    struct constraint* c;

    array_appendu(&current_hop->corrupted, value);
}

static uint32_t find_type_from_constraint(uint32_t attr)
{
    /* Looks through the registers and finds a concrete register implementing
     * that attribute, and returns the type. We assume that all registers
     * implementing an attribute (which anyone is going to ask for, 'volatile'
     * doesn't count) will have the same type. TODO: mcgg should check for
     * this. */

    const struct burm_register_data* brd = burm_register_data;
    while (brd->id)
    {
        if (brd->attrs & attr)
        {
            const uint32_t type_attrs =
                (burm_int_ATTR | burm_float_ATTR |
                 burm_long_ATTR | burm_double_ATTR);

            if (brd->attrs & type_attrs)
                return brd->attrs & type_attrs;
            return attr;
        }
        brd++;
    }

    fatal("unable to find a register matching attribute 0x%x", attr);
    return 0;
}

static void constrain_output_reg(uint32_t attr)
{
    struct value* value = &current_insn->value;

    array_appendu(&current_hop->outputs, value);
    value->attrs = find_type_from_constraint(attr);
}

static void constrain_output_reg_equal_to(int child)
{
    struct value* value = find_value_of_child(child);
    pmap_add(&current_hop->equals_constraint, &current_hop->value, value);
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
    &constrain_input_reg_corrupted,
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
    struct insn* insn = heap_alloc(&proc_heap, 1, sizeof(*insn));
    int i;

    insn->value.ir = current_ir;
    /* hop IDs count in preorder, so the result is always in $thing:0. */
    insn->value.subid = hop_id++;

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

        tracef('I', "I: $%d:%d for node $%d goal %d %s selected %d: %s\n",
            insn->value.ir->id, insn->value.subid,
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

            current_hop->value = &insn->value;
            switch (node->label)
            {
                case ir_to_esn(IR_REG, 0):
                    current_hop->value = &node->ir->value;
                    break;

                case ir_to_esn(IR_NOP, 'I'):
                case ir_to_esn(IR_NOP, 'F'):
                case ir_to_esn(IR_NOP, 'L'):
                case ir_to_esn(IR_NOP, 'D'):
                    array_appendu(&current_hop->inputs, &insn->children[0]->value);
                    array_appendu(&current_hop->outputs, current_hop->value);
                    hop_add_insel(current_hop, "@copy %V %V", &insn->children[0]->value, current_hop->value);
                    break;
            }

            hop_print('I', current_hop);
            array_append(&current_bb->hops, current_hop);
        }
    }

    return insn;
}

static struct burm_node* build_shadow_tree(struct ir* root, struct ir* ir)
{
    struct burm_node* node = heap_alloc(&proc_heap, 1, sizeof(*node));
    node->ir = ir;

    if (ir->root == root)
    {
        node->label = ir_to_esn(ir->opcode, ir->type);

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
	int i, j;

	tracef('I', "I: BLOCK: %s\n", current_bb->name);

    for (i=0; i<current_bb->irs.count; i++)
    {
		current_ir = current_bb->irs.item[i];
        if (current_ir->opcode != IR_PHI)
            break;

        tracef('I', "I: $%d is phi:", current_ir->id);
        for (j=0; j<current_ir->u.phivalue.count; j++)
        {
            struct basicblock* parentbb = current_ir->u.phivalue.item[j].left;
            struct ir* parentir = current_ir->u.phivalue.item[j].right;
            tracef('I', " %s=>$%d", parentbb->name, parentir->id);

            pmap_add(&current_bb->imports, parentir, current_ir);
            pmap_add(&parentbb->exports, parentir, current_ir);
        }
        tracef('I', "\n");
    }

	for (; i<current_bb->irs.count; i++)
	{
        struct burm_node* shadow;
        int insnno;

		current_ir = current_bb->irs.item[i];
        assert(current_ir->opcode != IR_PHI);

        ir_print('I', current_ir);
        shadow = build_shadow_tree(current_ir, current_ir);
        burm_label(shadow);

        insnno = burm_rule(shadow->state_label, 1);
        if (!insnno)
            burm_panic_cannot_match(shadow);

        hop_id = 0;
        walk_instructions(shadow, burm_stmt_NT);
	}
}

void pass_instruction_selector(void)
{
    int i;

    for (i=0; i<dominance.preorder.count; i++)
    {
        current_bb = dominance.preorder.item[i];
        select_instructions();
    }
}

/* vim: set sw=4 ts=4 expandtab : */

