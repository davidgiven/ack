#include "em_private.h"

CC_dnam(str, val)
	char *str;
	arith val;
{
	COMMA();
	NOFF(str, val);
}
