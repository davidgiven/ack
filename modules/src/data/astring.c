#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

const char* aprintf(const char* fmt, ...)
{
    int n;
    char* p;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);

    p = malloc(n);
    if (!p)
        return NULL;

    va_start(ap, fmt);
    vsnprintf(p, n, fmt, ap);
    va_end(ap);

    return p;
}

/* vim: set sw=4 ts=4 expandtab : */

