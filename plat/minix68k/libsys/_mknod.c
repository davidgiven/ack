#include "lib.h"
#define mknod _mknod
#include <unistd.h>

PUBLIC int mknod(const char* name, int mode, int addr)
{
	return (_callm1(FS, MKNOD, _len(name), mode, addr, (char*)name, (char*)0, NIL_PTR));
}
