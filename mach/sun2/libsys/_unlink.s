#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __unlink
__unlink: SYSTEM(SYS_unlink)
