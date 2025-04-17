#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "test.h"

int main(int argc, const char* argv[])
{
    char* o;
    char* p;

    p = sbrk(0);
    ASSERT(p == sbrk(0));
    ASSERT(p == sbrk(8));
    ASSERT(p != sbrk(0));
    ASSERT(p != sbrk(-8));
    ASSERT(p == sbrk(0));

    errno = 0;
    o = sbrk(INT_MAX);
    if (o == (char*)-1)
        ASSERT(ENOMEM == errno);
    else
    {
        ASSERT(0 == errno);
        p = sbrk(0);
        ASSERT(p > o);
        brk(o);
    }

    errno = 0;
    o = sbrk(INT_MIN);
    if (o == (char*)-1)
        ASSERT(ENOMEM == errno);
    else
    {
        ASSERT(0 == errno);
        p = sbrk(0);
        ASSERT(p < o);
        brk(o);
    }

    finished();
}

