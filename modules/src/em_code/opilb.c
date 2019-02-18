#include "em_private.h"

/* $Id$ */

void CC_opilb(int opcode, label ilb)
{
	/*	Instruction with instruction label argument
		Argument types: b
	*/
	OP(opcode);
	CILB(ilb);
	NL();
}
