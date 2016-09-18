#include "mcg.h"

static int next_id = 0;

struct ir* new_ir0(int opcode, int size)
{
	struct ir* ir = calloc(sizeof(struct ir), 1);
	ir->id = next_id++;
	ir->opcode = opcode;
	ir->size = size;

	switch (ir->opcode)
	{
		case IR_JUMP:
		case IR_CJUMP:
			ir->terminates = true;
			break;
	}

	return ir;
}

struct ir* new_ir1(int opcode, int size,
	struct ir* c1)
{
	struct ir* ir = new_ir0(opcode, size);
	ir->children[0] = c1;
	return ir;
}

struct ir* new_ir2(int opcode, int size,
	struct ir* c1, struct ir* c2)
{
	struct ir* ir = new_ir0(opcode, size);
	ir->children[0] = c1;
	ir->children[1] = c2;
	return ir;
}

struct ir* new_ir3(int opcode, int size,
	struct ir* c1, struct ir* c2, struct ir* c3)
{
	struct ir* ir = new_ir0(opcode, size);
	ir->children[0] = c1;
	ir->children[1] = c2;
	ir->children[2] = c3;
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
	int i;
	for (i=0; i<sizeof(ir->children)/sizeof(*ir->children); i++)
	{
		if (ir->children[i])
			ir_print(ir->children[i]);
	}

	printf("\t; %c ",
		ir->sequence ? 'S' : ' ');
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
			for (i=0; i<sizeof(ir->children)/sizeof(*ir->children); i++)
			{
				if (ir->children[i])
				{
					if (i > 0)
						printf(", ");
					printf("$%d", ir->children[i]->id);
				}
			}
	}

	printf(")\n");
}

/* vim: set sw=4 ts=4 expandtab : */
