#include "em_private.h"

/* $Id$ */

void
CC_op(opcode)
{
	/*	Instruction that never has an argument
		Argument types: -
	*/
	OP(opcode);
	NL();
}
