#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getpriority
_getpriority: SYSTEM(SYS_getpriority)
