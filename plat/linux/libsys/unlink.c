#include <unistd.h>
#include "libsys.h"

int unlink(const char* pathname)
{
    return _syscall(__NR_unlink, (quad) pathname, 0, 0);
}

