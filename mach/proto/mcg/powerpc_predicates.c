#include "mcg.h"

bool burm_predicate_constant_signed_16_bit(struct burm_node* node)
{
	struct ir* ir = node->ir;
	assert(ir->opcode == IR_CONST);
	return (ir->u.ivalue >= -0x8000) && (ir->u.ivalue <= 0x7fff);
}

bool burm_predicate_constant_is_zero(struct burm_node* node)
{
	struct ir* ir = node->ir;
	assert(ir->opcode == IR_CONST);
	return (ir->u.ivalue == 0);
}

/* vim: set sw=4 ts=4 expandtab : */


