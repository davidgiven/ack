#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _execve
_execve: SYSTEM(SYS_execve)
