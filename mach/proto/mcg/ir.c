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

struct ir* new_wordir(arith value)
{
	struct ir* ir = new_ir0(IR_ICONST, EM_wordsize);
	ir->u.ivalue = value;
	return ir;
}

struct ir* new_regir(int reg)
{
	struct ir* ir = new_ir0(IR_REG, EM_pointersize);
	ir->u.rvalue = reg;
	return ir;
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

struct ir* new_phiir(int size)
{
	return new_ir0(IR_PHI, size);
}

void ir_print(const struct ir* ir)
{
    if (ir->left)
        ir_print(ir->left);
    if (ir->right)
        ir_print(ir->right);

	printf("\t; %c ",
		ir->is_sequence ? 'S' : ' ');
	printf("$%d = ", ir->id);
	printf("%s%d(",
		ir_names[ir->opcode],
		ir->size);

	switch (ir->opcode)
	{
		case IR_ICONST:
			printf("%d", ir->u.ivalue);
			break;

		case IR_LABEL:
			printf("%s", ir->u.lvalue);
			break;

		case IR_REG:
			printf("%d", ir->u.rvalue);
			break;

		case IR_BLOCK:
			printf("%s", ir->u.bvalue->name);
			break;

		default:
            if (ir->left)
                printf("$%d", ir->left->id);
            if (ir->right)
                printf(", $%d", ir->right->id);
	}

	printf(")\n");
}

/* vim: set sw=4 ts=4 expandtab : */
