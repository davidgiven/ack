#include "system.h"
#include <fcntl.h>

#if defined WIN32
#include <io.h>
#endif

void sys_setbinarymode(FILE* fp)
{
    #if defined WIN32
        setmode(fileno(fp), O_BINARY);
    #endif
}

