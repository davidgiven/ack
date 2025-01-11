#include "lib.h"
#define access _access
#include <unistd.h>

PUBLIC int access(const char* name, int mode)
{
	return (_callm3(FS, ACCESS, mode, name));
}
