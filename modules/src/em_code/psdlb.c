#include "em_private.h"

CC_psdlb(op, dlb)
	label dlb;
{
	/*	Pseudo with numeric datalabel
	*/
	PS(op);
	DLB(dlb);
	NL();
}
