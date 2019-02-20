#include "em_private.h"

/* $Id$ */

void CC_oppnam(int opcode, char* pnam)
{
	/*	Instruction that has a procedure name as argument
		Argument types: p
	*/
	OP(opcode);
	PNAM(pnam);
	NL();
}
