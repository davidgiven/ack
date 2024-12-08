#include <system.h>
#include "back.h"

void
not_implemented( instr)
char *instr;
{
	fprint( stderr, "!!	%s, NOT implemented	!!\n", instr);
}
