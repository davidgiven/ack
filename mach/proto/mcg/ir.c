#include "mcg.h"

static int next_id = 0;
static struct mempool irpool;
static struct buffer renderbuf;

void clear_ir(void)
{
    tracef('M', "M: ir mempool was %d bytes\n", irpool.size);
    mempool_reset(&irpool);
    next_id = 0;
}

struct ir* new_ir0(int opcode, int size)
{
	struct ir* ir = mempool_alloc(&irpool, sizeof(struct ir));
	ir->id = next_id++;
	ir->opcode = opcode;
	ir->size = size;
	return ir;
}

struct ir* new_ir1(int opcode, int size, struct ir* left)
{
	struct ir* ir = new_ir0(opcode, size);
	ir->left = left;
	return ir;
}

struct ir* new_ir2(int opcode, int size, struct ir* left, struct ir* right)
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

struct ir* new_longir(arith value)
{
	return new_constir(EM_wordsize * 2, value);
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

struct ir* ir_walk(struct ir* ir, ir_walker_t* cb, void* user)
{
	assert(ir->root);
	if (cb(ir, user))
		return ir;

	if (ir->left && (ir->left->root == ir->root))
	{
		struct ir* irr = ir_walk(ir->left, cb, user);
		if (irr)
			return irr;
	}

	if (ir->right && (ir->right->root == ir->root))
	{
		struct ir* irr = ir_walk(ir->right, cb, user);
		if (irr)
			return irr;
	}

	return NULL;
}

static bool finder_cb(struct ir* ir, void* user)
{
	int opcode = *(int*)user;
	if (ir->opcode == opcode)
		return true;
	return false;
}

struct ir* ir_find(struct ir* ir, int opcode)
{
	return ir_walk(ir, finder_cb, &opcode);
}

static void render_expr(const struct ir* ir)
{
	buffer_appendf(&renderbuf, "%s", ir_data[ir->opcode].name);
	if (ir->type)
		buffer_appendf(&renderbuf, ".%c", ir->type);
	else if (ir->size)
		buffer_appendf(&renderbuf, "%d", ir->size);
	buffer_appendf(&renderbuf, "(");

	switch (ir->opcode)
	{
		case IR_CONST:
		case IR_LOCAL:
			buffer_appendf(&renderbuf, "%d", ir->u.ivalue);
			break;

		case IR_LABEL:
			buffer_appendf(&renderbuf, "%s", ir->u.lvalue);
			break;

		case IR_BLOCK:
			buffer_appendf(&renderbuf, "%s", ir->u.bvalue->name);
			break;

		case IR_PHI:
		{
			int i;

			for (i = 0; i < ir->u.phivalue.count; i++)
			{
				if (i > 0)
					buffer_appendf(&renderbuf, ", ");
				buffer_appendf(&renderbuf,
				    "%s=>$%d", ir->u.phivalue.item[i].left->name,
				    ir->u.phivalue.item[i].right->id);
			}
			break;
		}

		default:
			if (ir->left)
			{
				if (ir->left->root != ir->root)
					buffer_appendf(&renderbuf, "$%d", ir->left->id);
				else
					render_expr(ir->left);
			}
			if (ir->right)
			{
				buffer_appendf(&renderbuf, ", ");
				if (ir->right->root != ir->root)
					buffer_appendf(&renderbuf, "$%d", ir->right->id);
				else
					render_expr(ir->right);
			}
	}

	buffer_appendf(&renderbuf, ")");
}

const char* ir_render(const struct ir* ir)
{
	buffer_clear(&renderbuf);
    buffer_appendf(&renderbuf, "$%d = ", ir->id);
    render_expr(ir);
    return renderbuf.ptr;
}

void ir_print(char k, const struct ir* ir)
{
	tracef(k, "%c: %s\n", k, ir_render(ir));
}

/* vim: set sw=4 ts=4 expandtab : */
