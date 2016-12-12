#include "mcg.h"

static int next_id = 0;

struct ir* new_ir0(int opcode, int size)
{
	struct ir* ir = calloc(1,  sizeof(struct ir));
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

static void print_expr(char k, const struct ir* ir)
{
    tracef(k, "%s", ir_data[ir->opcode].name);
    if (ir->type)
        tracef(k, ".%c", ir->type);
    else if (ir->size)
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

            for (i=0; i<ir->u.phivalue.count; i++)
            {
                if (i > 0)
                    tracef(k, ", ");
                tracef(k, "%s=>$%d",
                    ir->u.phivalue.item[i].left->name,
                    ir->u.phivalue.item[i].right->id);
            }
            break;
        }

		default:
            if (ir->left)
            {
                if (ir->left->root != ir->root)
                    tracef(k, "$%d", ir->left->id);
                else
                    print_expr(k, ir->left);
            }
            if (ir->right)
            {
                tracef(k, ", ");
                if (ir->right->root != ir->root)
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

struct replacement_data
{
    struct ir* src;
    struct ir* dest;
    struct basicblock* source_bb;
};

static bool replace_irs_cb(struct ir* ir, void* user)
{
    struct replacement_data* rd = user;

    if (ir->left == rd->src)
        ir->left = rd->dest;
    if (ir->right == rd->src)
        ir->right = rd->dest;

    return false;
}

static bool replace_phis_cb(struct ir* ir, void* user)
{
    int i;
    struct replacement_data* rd = user;

    if (ir->opcode != IR_PHI)
        return true;

    for (i=0; i<ir->u.phivalue.count; i++)
    {
        if (ir->u.phivalue.item[i].right == rd->src)
        {
            ir->u.phivalue.item[i].right = rd->dest;
            ir->u.phivalue.item[i].left = rd->source_bb;
        }
    }

    return false;
}

/* Replaces IR references in this block, and any phis referring to the source IR in
 * the block's successors --- but no further. */
void ir_rewrite_single_block(struct basicblock* bb, struct ir* src, struct ir* dest)
{
    int i, j;
    struct replacement_data rd;

    rd.src = src;
    rd.dest = dest;
    rd.source_bb = bb;

    for (i=0; i<bb->irs.count; i++)
    {
        struct ir* ir = bb->irs.item[i];
        ir_walk(ir, replace_irs_cb, &rd);
    }

    for (i=0; i<bb->nexts.count; i++)
    {
        struct basicblock* nextbb = bb->nexts.item[i];

        for (j=0; j<nextbb->irs.count; j++)
        {
            struct ir* ir = nextbb->irs.item[j];
            ir_walk(ir, replace_phis_cb, &rd);
        }
    }
}

/* Rewrites IR references in all blocks downstream in the dominance graph, and all phis
 * in all nexts (which may be backreferences). */
void ir_rewrite(struct basicblock* bb, struct ir* src, struct ir* dest)
{
    int i;

    ir_rewrite_single_block(bb, src, dest);

    for (i=0; i<dominance.graph.count; i++)
    {
        struct basicblock* left = dominance.graph.item[i].left;
        struct basicblock* right = dominance.graph.item[i].right;
        if ((right == bb) && (left != right))
            ir_rewrite(left, src, dest);
    }
}

/* vim: set sw=4 ts=4 expandtab : */
