#include "lib.h"
#define creat _creat
#include <fcntl.h>

PUBLIC int creat(const char* name, mode_t mode)
{
	return (_callm3(FS, CREAT, mode, name));
}
