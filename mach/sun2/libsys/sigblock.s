#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sigblock
_sigblock: SYSTEM(SYS_sigblock)
