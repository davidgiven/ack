#include "em_private.h"

/* $Header$ */

void
CC_opcst(opcode, cst)
	arith cst;
{
	/*	Instruction with a constant argument
		Argument types: c, d, l, g, f, n, s, z, o, w, r
	*/
	OP(opcode);
	CST(cst);
	NL();
}
