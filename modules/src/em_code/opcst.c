#include "em_private.h"

/* $Id$ */

void CC_opcst(int opcode, arith cst)
{
	/*	Instruction with a constant argument
		Argument types: c, d, l, g, f, n, s, z, o, w, r
	*/
	OP(opcode);
	CST(cst);
	NL();
}
