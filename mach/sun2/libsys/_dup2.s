#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __dup2
__dup2: SYSTEM(SYS_dup2)
