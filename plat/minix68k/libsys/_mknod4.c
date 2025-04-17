#include "lib.h"
#define mknod4 _mknod4
#include <unistd.h>

PUBLIC int mknod4(const char* name, int mode, int addr, unsigned int size)
{
	return (_callm1(FS, MKNOD, _len(name), mode, addr, (char*)name, (char*)size, NIL_PTR));
}
