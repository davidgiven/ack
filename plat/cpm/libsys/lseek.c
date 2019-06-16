#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <cpm.h>
#include "cpmsys.h"

off_t lseek(int fd, off_t offset, int whence)
{
    struct FCBE* fcbe = &__fd[fd];

    __init_file_descriptors();
    if (fcbe->fcb.dr == 0)
    {
        /* Can't seek the console. */
        return 0;
    }

    if (whence == SEEK_END)
        offset += fcbe->length<<7;
    if (whence == SEEK_CUR)
        offset += (U16(fcbe->fcb.r)<<7) | fcbe->offset;
    
    U16(fcbe->fcb.r) = offset>>7;
    fcbe->offset = offset & 0x7f;
    return 0;
}
