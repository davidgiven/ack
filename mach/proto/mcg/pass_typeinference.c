#include "mcg.h"

static bool changed;
static ARRAYOF(struct ir) irs;

static void addall(struct ir* ir)
{
    if (array_appendu(&irs, ir))
        return;

    if (ir->left)
        addall(ir->left);
    if (ir->right)
        addall(ir->right);
}

static void collect_irs(void)
{
    int i;
    
    irs.count = 0;
	for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];
        int j;

        for (j=0; j<bb->irs.count; j++)
            addall(bb->irs.item[j]);
    }
}

static char effective_type(struct ir* ir, char type)
{
	switch (type)
	{
		case 'I':
		case 'F':
		case 'L':
		case 'D':
			return type;

		case 'i':
			if (ir->size == EM_wordsize)
				return 'I';
			else if (ir->size == (EM_wordsize*2))
				return 'L';
			break;

		case 'f':
			if (ir->size == EM_wordsize)
				return 'F';
			else if (ir->size == (EM_wordsize*2))
				return 'D';
			break;
	}
	
	return 0;
}

static void scan_irs(void)
{
	int i;

	for (i=0; i<irs.count; i++)
	{
		struct ir* ir = irs.item[i];

		if (ir->opcode == IR_PHI)
		{
			int j;

			/* Phis are special. We treat them as ?=? for every import value.
			 * */

			if (ir->type)
			{
				/* Push this type to all our children. */

				for (j=0; j<ir->u.phivalue.count; j++)
				{
					struct ir* child = ir->u.phivalue.item[j].right;
					if (!child->type)
					{
						child->type = ir->type;
						changed = true;
					}
				}
			}
			else
			{
				/* Pull our type from the first child with a set type; we
				 * ignore the rest, as the next iteration will push our type to
				 * them. It's possible for our children to have conflicting
				 * types. There's not much we can do about that so we just have
				 * to live with it and intelligently insert casts. */

				for (j=0; j<ir->u.phivalue.count; j++)
				{
					struct ir* child = ir->u.phivalue.item[j].right;
					if (child->type)
					{
						/* Found one! */
						ir->type = child->type;
						changed = true;
						break;
					}
				}
			}
		}
		else
		{
			const struct ir_data* ird = &ir_data[ir->opcode];

			if (!ir->type)
			{
				char etype = effective_type(ir, ird->returntype);
				if (etype)
				{
					ir->type = etype;
					changed = true;
				}
			}

			if (ir->left && !ir->left->type)
			{
				const struct ir_data* leftird = &ir_data[ir->left->opcode];
				if (leftird->returntype == '?')
				{
					char etype = effective_type(ir, ird->lefttype);
					if (etype)
					{
						ir->left->type = etype;
						changed = true;
					}
				}
			}

			if (ir->right && !ir->right->type)
			{
				const struct ir_data* rightird = &ir_data[ir->right->opcode];
				if (rightird->returntype == '?')
				{
					char etype = effective_type(ir, ird->righttype);
					if (etype)
					{
						ir->right->type = etype;
						changed = true;
					}
				}
			}

			if (!ir->type && (ird->returntype == '?'))
			{
				if ((ird->lefttype == '?') && ir->left->type)
				{
					ir->type = ir->left->type;
					changed = true;
				}

				if ((ird->righttype == '?') && ir->right->type)
				{
					ir->type = ir->right->type;
					changed = true;
				}
			}

			if (ir->type && (ird->lefttype == '?') && !ir->left->type)
			{
				ir->left->type = ir->type;
				changed = true;
			}

			if (ir->type && (ird->righttype == '?') && !ir->right->type)
			{
				ir->right->type = ir->type;
				changed = true;
			}
		}
	}
}

static void propagate_types(void)
{
	do
	{
		changed = false;

		scan_irs();
	}
	while (changed);
}

static void assign_fallback_types(void)
{
	int i;

	for (i=0; i<irs.count; i++)
	{
		struct ir* ir = irs.item[i];
		const struct ir_data* ird = &ir_data[ir->opcode];

		if (!ir->type && (ird->returntype == '?'))
			ir->type = effective_type(ir, 'i');
	}
}

static struct ir* new_copy(char wanted, char real, struct ir* ir)
{
	struct ir* copy;
	int opcode;

	if ((wanted == 'F') && (real == 'I'))
		opcode = IR_COPYI;
	else if ((wanted == 'D') && (real == 'L'))
		opcode = IR_COPYL;
	else if ((wanted == 'I') && (real == 'F'))
		opcode = IR_COPYF;
	else if ((wanted == 'L') && (real == 'D'))
		opcode = IR_COPYD;
	else
		fatal("type mismatch: parent IR wanted %c, child IR provided %c",
			wanted, real);

	copy = new_ir1(opcode, ir->size, ir);
	copy->type = wanted;
	return copy;
}

static void insert_copies(void)
{
	int i;

	/* Insert copies for normal IR nodes. */

	for (i=0; i<irs.count; i++)
	{
		struct ir* ir = irs.item[i];
		const struct ir_data* ird = &ir_data[ir->opcode];

		if (ir->left)
		{
			char wanted = effective_type(ir, ird->lefttype);
			char real = ir->left->type;

			if (wanted && (wanted != real))
			{
				struct ir* copy = new_copy(wanted, real, ir->left);
				copy->root = ir->root;
				ir->left = copy;
			}
		}

		if (ir->right)
		{
			char wanted = effective_type(ir, ird->righttype);
			char real = ir->right->type;

			if (wanted && (wanted != real))
			{
				struct ir* copy = new_copy(wanted, real, ir->right);
				copy->root = ir->root;
				ir->right = copy;
			}
		}
	}
}

void pass_infer_types(void)
{
	collect_irs();
	propagate_types();
	assign_fallback_types();
	insert_copies();
}

/* vim: set sw=4 ts=4 expandtab : */

