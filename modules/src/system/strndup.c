#include <stdlib.h>
#include <string.h>
#include "system.h"

#if defined WIN32
/* Really? */
char* strndup(const char* s, size_t n)
{
    size_t len = strnlen(s, n);
    char* ns = malloc(len + 1);
    if (!ns)
        return NULL;

    ns[len] = '\0';
    memcpy(ns, s, len);
    return ns;
}
#endif

