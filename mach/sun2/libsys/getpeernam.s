#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getpeername
_getpeername: SYSTEM(SYS_getpeername)
