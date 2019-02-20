#include "em_private.h"

/* $Id$ */

void CC_psdlb(int op, label dlb)
{
	/*	Pseudo with numeric datalabel
	*/
	PS(op);
	DLB(dlb);
	NL();
}
