#include "lib.h"
#define chroot _chroot
#include <unistd.h>

PUBLIC int chroot(const char* name)
{
	return (_callm3(FS, CHROOT, 0, name));
}
