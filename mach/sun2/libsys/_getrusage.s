#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __getrusage
__getrusage: SYSTEM(SYS_getrusage)
