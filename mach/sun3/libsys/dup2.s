#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _dup2
_dup2: SYSTEM(SYS_dup2)
