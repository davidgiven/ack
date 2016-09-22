#include "mcg.h"

static int next_id = 0;

struct ir* new_ir0(int opcode, int size)
{
	struct ir* ir = calloc(sizeof(struct ir), 1);
	ir->id = next_id++;
	ir->opcode = opcode;
	ir->size = size;
	return ir;
}

struct ir* new_ir1(int opcode, int size,
	struct ir* left)
{
	struct ir* ir = new_ir0(opcode, size);
	ir->left = left;
	return ir;
}

struct ir* new_ir2(int opcode, int size,
	struct ir* left, struct ir* right)
{
	struct ir* ir = new_ir0(opcode, size);
	ir->left = left;
	ir->right = right;
	return ir;
}

struct ir* new_labelir(const char* label)
{
	struct ir* ir = new_ir0(IR_LABEL, EM_pointersize);
	ir->u.lvalue = label;
	return ir;
}

struct ir* new_constir(int size, arith value)
{
	struct ir* ir = new_ir0(IR_CONST, size);
	ir->u.ivalue = value;
	return ir;
}

struct ir* new_wordir(arith value)
{
    return new_constir(EM_wordsize, value);
}

struct ir* new_bbir(struct basicblock* bb)
{
	struct ir* ir = new_ir0(IR_BLOCK, EM_pointersize);
	ir->u.bvalue = bb;
	return ir;
}

struct ir* new_anyir(int size)
{
	return new_ir0(IR_ANY, size);
}

struct ir* new_localir(int offset)
{
    struct ir* ir = new_ir0(IR_LOCAL, EM_pointersize);
    ir->u.ivalue = offset;
    return ir;
}

struct ir* ir_find(struct ir* ir, int opcode)
{
    if (ir->opcode == opcode)
        return ir;

    if (ir->left && !ir->left->is_sequence)
    {
        struct ir* irr = ir_find(ir->left, opcode);
        if (irr)
            return irr;
    }

    if (ir->right && !ir->right->is_sequence)
    {
        struct ir* irr = ir_find(ir->right, opcode);
        if (irr)
            return irr;
    }

    return NULL;
}

static void print_expr(char k, const struct ir* ir)
{
    tracef(k, "%s", ir_names[ir->opcode]);
    if (ir->size)
        tracef(k, "%d", ir->size);
    tracef(k, "(");

	switch (ir->opcode)
	{
		case IR_CONST:
        case IR_LOCAL:
			tracef(k, "%d", ir->u.ivalue);
			break;

		case IR_LABEL:
			tracef(k, "%s", ir->u.lvalue);
			break;

		case IR_BLOCK:
			tracef(k, "%s", ir->u.bvalue->name);
			break;

        case IR_PHI:
        {
            int i;

            for (i=0; i<ir->u.phivalue.imports_count; i++)
            {
                if (i > 0)
                    tracef(k, ", ");
                tracef(k, "$%d", ir->u.phivalue.imports[i]->id);
            }
        }

		default:
            if (ir->left)
            {
                if (ir->left->is_sequence)
                    tracef(k, "$%d", ir->left->id);
                else
                    print_expr(k, ir->left);
            }
            if (ir->right)
            {
                tracef(k, ", ");
                if (ir->right->is_sequence)
                    tracef(k, "$%d", ir->right->id);
                else
                    print_expr(k, ir->right);
            }
	}

	tracef(k, ")");
}

void ir_print(char k, const struct ir* ir)
{
	tracef(k, "%c: $%d = ", k, ir->id);
    print_expr(k, ir);
    tracef(k, "\n");
}

/* vim: set sw=4 ts=4 expandtab : */
