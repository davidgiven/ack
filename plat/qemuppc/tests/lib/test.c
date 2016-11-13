#include "test.h"

void finished(void)
{
    static const char s[] = "@@FINISHED\n";
    write(1, s, sizeof(s));
}

void writehex(uint32_t code)
{
    char buf[8];
    char* p = &buf[8];

    do
    {
        *--p = "0123456789abcdef"[code & 0xf];
        code >>= 4;
    }
    while (code > 0);

    write(1, p, buf+8-p);
}

void fail(uint32_t code)
{
    write(1, "@@FAIL ", 7);
    writehex(code);
    write(1, "\n", 1);
}
