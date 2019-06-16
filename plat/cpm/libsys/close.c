#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <cpm.h>
#include "cpmsys.h"

int close(int fd)
{
    struct FCBE* fcbe = &__fd[fd];
    int result = 0;
    uint8_t olduser;

    __init_file_descriptors();
    if (fcbe->fcb.f[0])
    {
        /* There's an actual filename here, so assume it's an open file. */

        olduser = cpm_get_user();
        cpm_set_user(fcbe->user);
        if (cpm_close_file(&fcbe->fcb) == 0xff)
        {
            errno = EIO;
            result = -1;
        }
        cpm_set_user(olduser);
    }

    memset(fcbe, 0, sizeof(struct FCBE));
    return result;
}

