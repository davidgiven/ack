#include <system.h>
#include "back.h"

void
not_implemented( instr)
char *instr;
{
	fprint( STDERR, "!!	%s, NOT implemented	!!\n", instr);
}
