#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <cpm.h>
#include "cpmsys.h"

struct FCBE __fd[NUM_FILE_DESCRIPTORS];
uint8_t __transfer_buffer[128];

void __init_file_descriptors(void)
{
    static uint8_t initialised = 0;
    if (!initialised)
    {
        /* Mark stdin, stdout, stderr as being open files. */
        __fd[0].fcb.f[0] = __fd[1].fcb.f[0] = __fd[2].fcb.f[0] = ' ';
        initialised = 1;
    }
}
