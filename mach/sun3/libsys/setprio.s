#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setpriority
_setpriority: SYSTEM(SYS_setpriority)
