#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _madvise
_madvise: SYSTEM(SYS_madvise)
