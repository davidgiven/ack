#include "lib.h"
#define stime _stime
#include <unistd.h>

PUBLIC int stime(long* top)
{
	_M.m2_l1 = *top;
	return (_callx(FS, STIME));
}
