#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __dup
__dup: SYSTEM(SYS_dup)
