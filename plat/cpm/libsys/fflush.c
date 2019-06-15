#include <stdio.h>
#include <errno.h>
#include <cpm.h>

int fflush(FILE* stream)
{
    if (stream == CPM_FAKE_FILE)
        return 0;
    
    errno = EBADF;
    return -1;
}