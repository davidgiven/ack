#include "em_private.h"

/* $Id$ */

void
CC_opdlb(opcode, dlb, offset)
	label dlb;
	arith offset;
{
	/*	Instruction that as a numeric datalabel + offset as argument
		Argument types: g
	*/
	OP(opcode);
	DOFF(dlb, offset);
	NL();
}
