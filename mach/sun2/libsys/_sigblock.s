#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __sigblock
__sigblock: SYSTEM(SYS_sigblock)
