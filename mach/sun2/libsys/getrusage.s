#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getrusage
_getrusage: SYSTEM(SYS_getrusage)
