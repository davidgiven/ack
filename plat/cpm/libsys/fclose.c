#include <stdio.h>
#include <errno.h>
#include <cpm.h>

int fclose(FILE* stream)
{
    if (stream == CPM_FAKE_FILE)
        return 0;
    
    if (fflush(stream))
        return;

    cpm_get_set_user(stream->user);
    if (cpm_close_file(&stream->fcb) == 0xff)
    {
        errno = EIO;
        return -1;
    }
    return 0;
}
