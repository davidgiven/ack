#include "em_private.h"

/* $Header$ */

void
CC_opnarg(opcode)
{
	/*	Instruction with optional argument, but now without one
		Argument types: w
	*/
	OP(opcode);
	CCEND();
	NL();
}
