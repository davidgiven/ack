#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __execve
__execve: SYSTEM(SYS_execve)
