#include "em_private.h"

/* $Header$ */

CC_psdlb(op, dlb)
	label dlb;
{
	/*	Pseudo with numeric datalabel
	*/
	PS(op);
	DLB(dlb);
	NL();
}
