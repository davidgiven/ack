#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "test.h"

int main(int argc, const char* argv[])
{
    void* p;

    ASSERT(-1 == (intptr_t)brk((void*)0xffffffff));
    ASSERT(ENOMEM == errno);

    p = sbrk(0);
    ASSERT(p == sbrk(0));
    ASSERT(p == sbrk(8));
    ASSERT(p != sbrk(0));
    ASSERT(p != sbrk(-8));
    ASSERT(p == sbrk(0));

    /* We assume the test environment has less than 2GB of RAM. */
    ASSERT(-1 == (intptr_t)sbrk(INT_MAX));
    ASSERT(-1 == (intptr_t)sbrk(INT_MIN));

    finished();
}

