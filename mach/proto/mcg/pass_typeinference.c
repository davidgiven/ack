#include "mcg.h"

static bool changed;
static struct set irs;

static void addall(struct ir* ir)
{
    if (set_add(&irs, ir))
        return;

    if (ir->left)
        addall(ir->left);
    if (ir->right)
        addall(ir->right);
}

static void collect_irs(void)
{
    int i;
    
    set_empty(&irs);
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
    struct set_iterator sit = {};

    while (set_next(&irs, &sit))
    {
		struct ir* ir = sit.item;

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
    struct set_iterator sit = {};

    while (set_next(&irs, &sit))
    {
		struct ir* ir = sit.item;
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
		fatal("type mismatch: parent IR $%d wanted %c, child IR provided %c",
			ir->id, wanted, real);

	copy = new_ir1(opcode, ir->size, ir);
	copy->type = wanted;
	return copy;
}

static void insert_copy(struct ir* ir, struct ir** child, char returntype, char childtype)
{
    if (*child)
    {
        char wanted;
        char real;

        if ((returntype == '?') && (childtype == '?'))
        {
            wanted = ir->type;
            real = (*child)->type;
        }
        else
        {
            wanted = effective_type(ir, childtype);
            real = (*child)->type;
        }

        if (wanted)
        {
            if (wanted != real)
            {
                struct ir* copy = new_copy(wanted, real, *child);
                copy->root = ir->root;
                *child = copy;
            }
        }
    }
}

static void insert_ir_copies(void)
{
    struct set_iterator sit = {};

	/* Insert copies for normal IR nodes. */

    while (set_next(&irs, &sit))
	{
		struct ir* ir = sit.item;
		const struct ir_data* ird = &ir_data[ir->opcode];

        insert_copy(ir, &ir->left, ird->returntype, ird->lefttype);
        insert_copy(ir, &ir->right, ird->returntype, ird->righttype);
	}
}

static void insert_phi_copies(void)
{
    struct set_iterator sit = {};
    int j;

    /* If the child of a phi isn't the same type as the phi itself, we need to
     * insert the copy at the end of the block that exported the value. */

    while (set_next(&irs, &sit))
    {
		struct ir* ir = sit.item;

        for (j=0; j<ir->u.phivalue.count; j++)
        {
            struct ir* childir = ir->u.phivalue.item[j].right;
            int wanted = ir->type;
            int real = childir->type;
            if (wanted != real)
            {
                struct basicblock* childbb = ir->u.phivalue.item[j].left;
                struct ir* copy = new_copy(wanted, real, childir);
                copy->root = copy;

                /* The copy gets inserted as the second last item of the child
                 * basic block. That way it'll happen before the final jump
                 * that exits the block. */

                array_insert(&childbb->irs, copy, childbb->irs.count-1);

                /* And replace the value in the phi with our copy. */

                ir->u.phivalue.item[j].right = copy;
            }
        }
    }
}

void pass_infer_types(void)
{
	collect_irs();
	propagate_types();
	assign_fallback_types();
	insert_ir_copies();
    insert_phi_copies();
}

/* vim: set sw=4 ts=4 expandtab : */

