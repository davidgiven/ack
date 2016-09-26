#include "mcg.h"

static enum ir_type search_for_type(struct ir* ir, enum ir_type desired)
{
	const struct ir_data* data;

	if (ir->type != IRT_UNSET)
		return ir->type;

	data = &ir_data[ir->opcode];
	if (ir->left)
		ir->left->type = search_for_type(ir->left, data->lefttype);
	if (ir->right)
		ir->right->type = search_for_type(ir->right, data->righttype);
	
    switch (data->returntype)
    {
        case IRT_ANY:
            if (desired == IRT_FLOAT)
                ir->opcode++;
            return desired;

        case IRT_UNSET:
            assert(!((data->lefttype == IRT_ANY) && (data->righttype == IRT_ANY)));
            if (((data->lefttype == IRT_ANY) && (ir->left->type == IRT_FLOAT)) ||
                ((data->righttype == IRT_ANY) && (ir->right->type == IRT_FLOAT)))
            {
                ir->opcode++;
                return IRT_FLOAT;
            }
            if ((data->lefttype == IRT_ANY) && (ir->left->type == IRT_ANY))
                ir->left->type = IRT_INT;
            if ((data->righttype == IRT_ANY) && (ir->right->type == IRT_ANY))
                ir->right->type = IRT_INT;
            return IRT_INT;

        default:
            return data->returntype;
	}
}

static void push_types_up(struct basicblock* bb)
{
	int i;

	for (i=0; i<bb->irs.count; i++)
	{
		struct ir* ir = bb->irs.item[i];
		ir->type = search_for_type(ir, ir->type);
	}
}

void pass_type_inference(struct procedure* proc)
{
    int i;
    
	for (i=0; i<proc->blocks.count; i++)
		push_types_up(proc->blocks.item[i]);
}

/* vim: set sw=4 ts=4 expandtab : */


