#include "mcg.h"

bool burm_predicate_signed_constant(struct burm_node* node, arith size)
{
	struct ir* ir = node->ir;
    arith pivot = 1<<(size-1);
    arith mask = ~((1<<size) - 1);
	assert(ir->opcode == IR_CONST);

    return ((ir->u.ivalue + pivot) & mask) == 0;
}

bool burm_predicate_unsigned_constant(struct burm_node* node, arith size)
{
	struct ir* ir = node->ir;
    arith mask = ~((1<<size) - 1);
	assert(ir->opcode == IR_CONST);

    return (ir->u.ivalue & mask) == 0;
}

bool burm_predicate_specific_constant(struct burm_node* node, arith val)
{
	struct ir* ir = node->ir;
	assert(ir->opcode == IR_CONST);

    return ir->u.ivalue == val;
}

bool burm_predicate_constant_within_inclusive_range(struct burm_node* node, arith min, arith max)
{
	struct ir* ir = node->ir;
	assert(ir->opcode == IR_CONST);

    return (ir->u.ivalue >= min) && (ir->u.ivalue <= max);
}

/* vim: set sw=4 ts=4 expandtab : */


