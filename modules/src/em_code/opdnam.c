#include "em_private.h"

/* $Id$ */

void CC_opdnam(int opcode, char* dnam, arith offset)
{
	/*	Instruction that has a datalabel + offset as argument
		Argument types: g
	*/
	OP(opcode);
	NOFF(dnam, offset);
	NL();
}
