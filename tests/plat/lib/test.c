#include "test.h"

/* No CRT in this file (this includes stdio and stdlib!). */

void finished(void)
{
    static const char s[] = "@@FINISHED\n";
    write(1, s, sizeof(s));
    _exit(0);
}

void writehex(uint32_t code)
{
    char buf[8];
    char* p = &buf[sizeof(buf)];

    do
    {
        *--p = "0123456789abcdef"[code & 0xf];
        code >>= 4;
    }
    while (code > 0);

    write(1, p, buf + sizeof(buf) - p);
}

void fail(uint32_t code)
{
    static const char fail_msg[] = "@@FAIL 0x";
    static const char nl_msg[] = "\n";

    write(1, fail_msg, sizeof(fail_msg)-1);
    writehex(code);
    write(1, nl_msg, sizeof(nl_msg)-1);
}
