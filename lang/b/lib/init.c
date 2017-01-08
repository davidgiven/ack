#include "b.h"
#include <stdarg.h>
#include <string.h>
#include <limits.h>

void binit(void)
{
    patch_addresses(bmodule_main);
}

