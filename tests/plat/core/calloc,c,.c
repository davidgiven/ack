#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "test.h"

int main(int argc, const char* argv[])
{
    const char* p;
    int i;

    ASSERT(0 == calloc(0, 0));
    ASSERT(0 == calloc(0, 1));
    ASSERT(0 == calloc(1, 0));
    ASSERT(0 == calloc(SIZE_MAX/2, 3));
    ASSERT(0 == calloc(SIZE_MAX/2, 2));
    ASSERT(0 != calloc(1, 1));

    p = calloc(10, 1);
    for (i=0; i<10; i++)
        ASSERT(0 == p[i]);

    finished();
}
