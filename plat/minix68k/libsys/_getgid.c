#include "lib.h"
#include <sys/types.h>
#define getgid _getgid
#include <unistd.h>

PUBLIC gid_t getgid(void)
{
	return ((gid_t)_callm1(MM, GETGID, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
