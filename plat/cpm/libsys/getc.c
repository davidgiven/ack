#include <stdio.h>
#include <errno.h>
#include <cpm.h>

struct buffer
{
    uint8_t mx;
    uint8_t nc;
    uint8_t c[128];
};

static struct buffer buffer;
static uint8_t pos;

int read_from_console(void)
{
    while (pos == buffer.nc)
    {
        /* Refill buffer. */
        buffer.mx = sizeof(buffer.c);
        buffer.nc = 0;
        cpm_readline((uint8_t*) &buffer);
        pos = 0;
    }

    return buffer.c[pos++];
}

int cpm_getc(FILE* stream)
{
    if (stream == CPM_FAKE_FILE)
        return read_from_console();

    errno = EBADF;
    return -1;
}
