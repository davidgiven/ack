#include "em_private.h"

/* $Id$ */

void
CC_msstart(cst)
	int cst;
{
	/*	start of message
	*/
	PS(ps_mes);
	CST((arith)cst);
}
