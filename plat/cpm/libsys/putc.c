#include <stdio.h>
#include <errno.h>
#include <cpm.h>

int cpm_putc(int c, FILE* stream)
{
    if (stream == CPM_FAKE_FILE)
    {
        cpm_conout(c);
        if (c == '\n')
            cpm_conout(c);
        return 0;
    }

    errno = EBADF;
    return -1;
}
