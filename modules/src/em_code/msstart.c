#include "em_private.h"

CC_msstart(cst)
	arith cst;
{
	/*	start of message
	*/
	PS(ps_mes);
	CST(cst);
}
