#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __fcntl
__fcntl: SYSTEM(SYS_fcntl)
